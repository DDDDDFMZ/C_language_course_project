//Airplane Flight Booking System(飞机航班订票系统)
//Features (功能):
//1. Flight info entry / browse / search		(航班信息录入/浏览/搜索)
//2. Passenger booking / selling / refund		(乘客订票/售票/退票)
//3. Seat simulation A/B/C/D, A/D = window		(座位模拟,A和D靠窗)
//4. Passenger list printing					(乘客清单打印)
//5. Statistics									(统计)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Macros (宏定义)
#define MAX_FLIGHTS       10	//Max number of flights         (最大航班数)
#define MAX_PASSENGERS   200    //Max passengers per flight     (每航班最大乘客数)
#define MAX_ROWS          50    //Max rows per flight           (每航班最大排数)
#define SEATS_PER_ROW      4    //Seats per row: A/B/C/D        (每排座位数)
#define MAX_NAME_LEN      20    //Max name length               (姓名最大长度)
#define MAX_ID_LEN        18    //Max ID card number length     (身份证号最大长度)
#define MAX_FLIGHT_NO_LEN 10    //Max flight number length      (航班号最大长度)
#define MAX_CITY_LEN      20    //Max city name length          (城市名最大长度)
#define MAX_TIME_LEN      20    //Max time string length        (时间字符串最大长度)

#define DATA_FILE "flights.dat"  //Data file for persistence  (数据持久化文件)
//Enums (枚举定义)

//Seat status (座位状态)
typedef enum {
    SEAT_AVAILABLE = 0,		//Free   (空闲)
    SEAT_BOOKED    = 1		//Booked (已预订)
} SeatStatus;

//User role for future use (用户角色, 预留)
typedef enum {
    ROLE_ADMIN     = 0,		//Admin      (管理员)
    ROLE_PASSENGER = 1		//Passenger  (乘客)
} UserRole;

//Structs (结构体定义)

//Seat info (座位信息)
typedef struct {
    int       row;			//Row number 1~MAX_ROWS       (排号)
    char      col;			//Column 'A'|'B'|'C'|'D'		(列号)
    SeatStatus status;		//Seat status                 (座位状态)
} Seat;

//Passenger info (乘客信息)
typedef struct {
    char name[MAX_NAME_LEN];	//Name             (姓名)
    char id  [MAX_ID_LEN];		//ID card number   (身份证号)
    int  seatRow;				//Seat row number  (座位排号)
    char seatCol;				//Seat column      (座位列号)
} Passenger;

//Flight info (航班信息)
typedef struct {
    char   flightNo[MAX_FLIGHT_NO_LEN];			//Flight number				(航班号)
    char   origin[MAX_CITY_LEN];				//Origin city				(起始地)
    char   destination[MAX_CITY_LEN];			//Destination city			(目的地)
    char   departureTime[MAX_TIME_LEN];			//Departure time			(出发时间)
    int    totalRows;							//Total seat rows			(总排数)
    int    totalSeats;							//Total seats=totalRows*4	(总座位数)
    int    bookedCount;							//Number of booked seats	(已订座位数)
    Seat   seats[MAX_ROWS][SEATS_PER_ROW];		//Seat matrix				(座位矩阵)
    Passenger passengers[MAX_PASSENGERS];		//Passenger list			(乘客列表)
    int    passengerCount;						//Current passenger count	(当前乘客数)
} Flight;

//System global state (系统全局状态)
typedef struct {
    Flight flights[MAX_FLIGHTS];
    int    flightCount;
} FlightSystem;

//Global Variables (全局变量)
static FlightSystem g_sys;

//Utility Function Declarations (工具函数声明)
static void clearInputBuffer(void);
static void pauseScreen(void);
static int  findFlightIndex(const char *flightNo);
static int  isWindowSeat(char col);
static void initSeats(Flight *f, int rows);

//Menu Function Declarations (菜单函数声明)
static void showMainMenu(void);
static void showAdminMenu(void);
static void showPassengerMenu(void);

//Admin Function Declarations (管理员功能声明)
static void adminAddFlight(void);       //1. Add flight info		(航班信息录入)
static void adminBrowseFlights(void);   //2. Browse all flights		(航班信息浏览)
static void adminSearchFlights(void);	//3. Search flights			(航班信息搜索)

//Passenger Function Declarations (乘客功能声明)
static void passengerBookTicket(void);		//4. Book ticket			(订票)
static void passengerSellTicket(void);		//5. Sell ticket			(售票)
static void passengerRefundTicket(void);	//6. Refund ticket			(退票)
static void passengerStatistics(void);		//7. Statistics				(统计)
static void passengerQuery(void);			//8. Query					(查询)

//Display Function Declarations (显示函数声明)
static void showSeatMap(const Flight *f);
static void printPassengerList(const Flight *f);

static void saveSystem(void);          //Save data to file (保存数据到文件)
static void loadSystem(void);          //Load data from file (从文件加载数据)
//Save all flight data to file (保存所有航班数据到文件)
static void saveSystem(void){
    FILE *fp;
    int i, j;
    fp = fopen(DATA_FILE, "w");
    if (fp == NULL){
        printf("[Warning] Cannot open file for saving. (警告) 无法打开文件写入.\n");
        return;
    }
    fprintf(fp, "%d\n", g_sys.flightCount);
    for (i = 0; i < g_sys.flightCount; i++){
        fprintf(fp, "F %s %s %s %s %d\n",
                g_sys.flights[i].flightNo,
                g_sys.flights[i].origin,
                g_sys.flights[i].destination,
                g_sys.flights[i].departureTime,
                g_sys.flights[i].totalRows);
        for (j = 0; j < g_sys.flights[i].passengerCount; j++){
            fprintf(fp, "P %s %s %d %c\n",
                    g_sys.flights[i].passengers[j].name,
                    g_sys.flights[i].passengers[j].id,
                    g_sys.flights[i].passengers[j].seatRow,
                    g_sys.flights[i].passengers[j].seatCol);
        }
    }
    fclose(fp);
}

//Load flight data from file (从文件加载航班数据)
static void loadSystem(void){
    FILE *fp;
    int flightCount, i, flIdx;
    char lineType;
    fp = fopen(DATA_FILE, "r");
    if (fp == NULL) return;
    if (fscanf(fp, "%d\n", &flightCount) != 1){
        fclose(fp);
        return;
    }
    for (i = 0; i < flightCount && g_sys.flightCount < MAX_FLIGHTS; i++){
        char fNo[MAX_FLIGHT_NO_LEN], orig[MAX_CITY_LEN], dest[MAX_CITY_LEN], dTime[MAX_TIME_LEN];
        int rows;
        if (fscanf(fp, " %c %s %s %s %s %d\n", &lineType, fNo, orig, dest, dTime, &rows) != 6) break;
        if (lineType != 'F') break;
        flIdx = g_sys.flightCount;
        strcpy(g_sys.flights[flIdx].flightNo, fNo);
        strcpy(g_sys.flights[flIdx].origin, orig);
        strcpy(g_sys.flights[flIdx].destination, dest);
        strcpy(g_sys.flights[flIdx].departureTime, dTime);
        initSeats(&g_sys.flights[flIdx], rows);
        g_sys.flightCount++;
        while (1){
            char pName[MAX_NAME_LEN], pId[MAX_ID_LEN];
            int sRow, pCount, r, c_idx;
            char sCol;
            if (fscanf(fp, " %c", &lineType) != 1){ fclose(fp); return; }
            if (lineType == 'F'){ ungetc(lineType, fp); break; }
            if (lineType != 'P') break;
            if (fscanf(fp, "%s %s %d %c\n", pName, pId, &sRow, &sCol) != 4) break;
            pCount = g_sys.flights[flIdx].passengerCount;
            if (pCount >= MAX_PASSENGERS) continue;
            strcpy(g_sys.flights[flIdx].passengers[pCount].name, pName);
            strcpy(g_sys.flights[flIdx].passengers[pCount].id, pId);
            g_sys.flights[flIdx].passengers[pCount].seatRow = sRow;
            g_sys.flights[flIdx].passengers[pCount].seatCol = sCol;
            g_sys.flights[flIdx].passengerCount++;
            r = sRow - 1;
            c_idx = (int)(sCol - 'A');
            if (r >= 0 && r < g_sys.flights[flIdx].totalRows &&
                c_idx >= 0 && c_idx < SEATS_PER_ROW){
                g_sys.flights[flIdx].seats[r][c_idx].status = SEAT_BOOKED;
                g_sys.flights[flIdx].bookedCount++;
            }
        }
    }
    fclose(fp);
}

//main() - Program entry point (程序入口)
int main(void){
    int choice;
    //Initialize system (初始化系统)
    g_sys.flightCount = 0;
    loadSystem();  //Load previously saved data (加载已保存数据)
    printf("========================================\n");
    printf("   Airplane Flight Booking System       \n");
    printf("   (飞机航班订票系统)                    \n");
    printf("========================================\n\n");

    while (1){
        showMainMenu();
        printf("Please select (请选择) (0-2): ");
        scanf("%d", &choice);
        clearInputBuffer();

        switch (choice){
        case 0:
            saveSystem();  //Save data before exit (退出前保存数据)
            printf("\nThank you for using the system. Goodbye!\n");
            printf("(感谢使用, 再见!)\n");
            return 0;

        case 1:
            //Admin portal (管理员入口)
            showAdminMenu();
            break;

        case 2:
            //Passenger portal (乘客入口)
            showPassengerMenu();
            break;

        default:
            printf("Invalid option, please try again! (无效选项, 请重新输入!)\n");
            break;
        }
    }
    return 0;
}

//Main Menu (主菜单)
static void showMainMenu(void){
    printf("\n");
    printf("+--------------------------------------+\n");
    printf("|            MAIN MENU (主菜单)         |\n");
    printf("+--------------------------------------+\n");
    printf("|  1. Admin Portal     (管理员入口)     |\n");
    printf("|  2. Passenger Portal (乘客入口)       |\n");
    printf("|  0. Exit System      (退出系统)       |\n");
    printf("+--------------------------------------+\n");
}

//Admin Menu (管理员菜单)
static void showAdminMenu(void){
    int choice;
    while (1){
        printf("\n");
        printf("+--------------------------------------+\n");
        printf("|           ADMIN MENU (管理员菜单)     |\n");
        printf("+--------------------------------------+\n");
        printf("|  1. Add Flight Info    (航班信息录入)  |\n");
        printf("|  2. Browse All Flights (航班信息浏览)  |\n");
        printf("|  3. Search Flights     (航班信息搜索)  |\n");
        printf("|  4. Save Data to File  (数据保存到文件)  |\n");
        printf("|  0. Return to Main Menu(返回主菜单)    |\n");
        printf("+--------------------------------------+\n");
        printf("Please select (请选择) (0-4): ");
        scanf("%d", &choice);
        clearInputBuffer();

        switch (choice){
        case 0: return;
        case 1: adminAddFlight();      break;
        case 2: adminBrowseFlights();  break;
        case 3: adminSearchFlights();  break;
        case 4: saveSystem(); printf("\n[OK] Data saved to file. (数据已保存到 flights.dat)\n"); pauseScreen(); break;
        default:
            printf("Invalid option, please try again! (无效选项, 请重新输入!)\n");
            break;
        }
    }
}

//Passenger Menu (乘客菜单)
static void showPassengerMenu(void){
    int choice;
    while (1){
        printf("\n");
        printf("+--------------------------------------+\n");
        printf("|         PASSENGER MENU (乘客菜单)     |\n");
        printf("+--------------------------------------+\n");
        printf("|  1. Book Ticket          (订票)       |\n");
        printf("|  2. Buy Ticket (Counter) (售票/现场)   |\n");
        printf("|  3. Refund Ticket        (退票)       |\n");
        printf("|  4. Statistics           (统计)       |\n");
        printf("|  5. Query (List/Flight)  (查询)       |\n");
        printf("|  0. Return to Main Menu  (返回主菜单)  |\n");
        printf("+--------------------------------------+\n");
        printf("Please select (请选择) (0-5): ");
        scanf("%d", &choice);
        clearInputBuffer();

        switch (choice){
        case 0: return;
        case 1: passengerBookTicket();   break;
        case 2: passengerSellTicket();   break;
        case 3: passengerRefundTicket(); break;
        case 4: passengerStatistics();   break;
        case 5: passengerQuery();        break;
        default:
            printf("Invalid option, please try again! (无效选项, 请重新输入!)\n");
            break;
        }
    }
}

//Utility Functions (工具函数)

//Clear the stdin input buffer (清空输入缓冲区)
static void clearInputBuffer(void){
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

//Pause and wait for user to press Enter (暂停, 按回车继续)
static void pauseScreen(void){
    printf("\nPress Enter to continue... (按回车键继续...)");
    clearInputBuffer();
}

//Find a flight's index by its flight number (根据航班号查找航班索引)
static int findFlightIndex(const char *flightNo){
    int i;
    for (i = 0; i < g_sys.flightCount; i++){
        if (strcmp(g_sys.flights[i].flightNo, flightNo) == 0)
            return i;
    }
    return -1;
}

//Check whether a seat column is a window seat (判断是否为靠窗座位)
//Seat layout: A(window) B(aisle) C(aisle) D(window)
static int isWindowSeat(char col){
    return (col == 'A' || col == 'D');
}

//Initialize the seat matrix for a flight (初始化航班座位矩阵)
static void initSeats(Flight *f, int rows){
    int r, c;
    (*f).totalRows  = rows;
    (*f).totalSeats = rows * SEATS_PER_ROW;
    (*f).bookedCount = 0;
    (*f).passengerCount = 0;
    for (r = 0; r < rows; r++){
        for (c = 0; c < SEATS_PER_ROW; c++){
            (*f).seats[r][c].row    = r + 1;
            (*f).seats[r][c].col    = (char)('A' + c);
            (*f).seats[r][c].status = SEAT_AVAILABLE;
        }
    }
}

//1. Add Flight Info (Admin) — (航班信息录入)
static void adminAddFlight(void){
    int rows;
    int addIdx = g_sys.flightCount;

    if (g_sys.flightCount >= MAX_FLIGHTS){
        printf("\n[Error] Maximum number of flights (%d) reached.\n", MAX_FLIGHTS);
        printf("(错误) 航班数量已达上限 (%d)。\n", MAX_FLIGHTS);
        pauseScreen();
        return;
    }

    printf("\n========== Add Flight Info (航班信息录入) ==========\n");
    printf("Enter flight number (输入航班号, e.g. CA1234): ");
    scanf("%s", g_sys.flights[addIdx].flightNo);
    clearInputBuffer();

    if (findFlightIndex(g_sys.flights[addIdx].flightNo) != -1){
        printf("[Error] Flight '%s' already exists! (航班 '%s' 已存在!)\n",
               g_sys.flights[addIdx].flightNo, g_sys.flights[addIdx].flightNo);
        return;
    }

    printf("Enter origin city (输入起始地, e.g. Beijing): ");
    scanf("%s", g_sys.flights[addIdx].origin);
    clearInputBuffer();

    printf("Enter destination city (输入目的地, e.g. Shanghai): ");
    scanf("%s", g_sys.flights[addIdx].destination);
    clearInputBuffer();

    printf("Enter departure time (输入出发时间, e.g. 2026-06-01 08:00): ");
    scanf("%[^\n]", g_sys.flights[addIdx].departureTime);
    clearInputBuffer();

    printf("Enter number of seat rows (输入座位排数) (1-%d, 4 seats/row A/B/C/D): ", MAX_ROWS);
    scanf("%d", &rows);
    clearInputBuffer();

    if (rows < 1 || rows > MAX_ROWS){
        printf("[Error] Rows must be between 1 and %d! (排数必须在 1~%d 之间!)\n", MAX_ROWS, MAX_ROWS);
        return;
    }

    initSeats(&g_sys.flights[addIdx], rows);
    g_sys.flightCount++;

    printf("\n[OK] Flight '%s' added! (航班 '%s' 录入成功!)\n",
           g_sys.flights[addIdx].flightNo, g_sys.flights[addIdx].flightNo);
    printf("     %d seats total (%d rows x 4). (共 %d 座, %d 排 x 4)\n",
           g_sys.flights[addIdx].totalSeats, g_sys.flights[addIdx].totalRows,
           g_sys.flights[addIdx].totalSeats, g_sys.flights[addIdx].totalRows);
    pauseScreen();
}

//2. Browse All Flights (Admin) — (航班信息浏览)
static void adminBrowseFlights(void){
    int i;

    printf("\n========== All Flights (全部航班信息) ==========\n");
    if (g_sys.flightCount == 0){
        printf("\nNo flight information available. (暂无航班信息)\n");
        pauseScreen();
        return;
    }

    for (i = 0; i < g_sys.flightCount; i++){
        printf("\n--------------------------------------\n");
        printf("  Flight No:    (航班号) %s\n", g_sys.flights[i].flightNo);
        printf("  Origin:       (起始地) %s\n", g_sys.flights[i].origin);
        printf("  Destination:  (目的地) %s\n", g_sys.flights[i].destination);
        printf("  Departure:    (出发时间) %s\n", g_sys.flights[i].departureTime);
        printf("  Total Seats:  (总座位) %d rows x 4 = %d\n",
               g_sys.flights[i].totalRows, g_sys.flights[i].totalSeats);
        printf("  Booked: %d (已订)  |  Available: %d (剩余)\n",
               g_sys.flights[i].bookedCount,
               g_sys.flights[i].totalSeats - g_sys.flights[i].bookedCount);
        printf("  Passengers:   (乘客数) %d\n", g_sys.flights[i].passengerCount);
    }
    pauseScreen();
}

//3. Search Flights (Admin & Passenger shared) — (航班信息搜索)
static void adminSearchFlights(void){
    int choice;
    char keyword[MAX_CITY_LEN];
    int i, found;

    if (g_sys.flightCount == 0){
        printf("\nNo flight information available. (暂无航班信息)\n");
        pauseScreen();
        return;
    }

    printf("\n========== Search Flights (航班信息搜索) ==========\n");
    printf("  1. Search by Flight Number   (按航班号搜索)\n");
    printf("  2. Search by Origin City     (按起始地搜索)\n");
    printf("  3. Search by Destination City(按目的地搜索)\n");
    printf("  0. Return (返回)\n");
    printf("Select search mode (选择搜索方式) (0-3): ");
    scanf("%d", &choice);
    clearInputBuffer();

    if (choice == 0) return;

    printf("Enter search keyword (输入搜索关键词): ");
    scanf("%s", keyword);
    clearInputBuffer();

    printf("\n========== Search Results (搜索结果) ==========\n");
    found = 0;
    for (i = 0; i < g_sys.flightCount; i++){
        int match = 0;
        switch (choice){
        case 1: match = (strcmp(g_sys.flights[i].flightNo, keyword) == 0); break;
        case 2: match = (strcmp(g_sys.flights[i].origin, keyword) == 0); break;
        case 3: match = (strcmp(g_sys.flights[i].destination, keyword) == 0); break;
        default:
            printf("[Error] Invalid search mode. (无效的搜索方式)\n");
            return;
        }
        if (match){
            found++;
            printf("\n  [%d] Flight (航班): %s\n", found, g_sys.flights[i].flightNo);
            printf("      Route (航线): %s --> %s\n",
                   g_sys.flights[i].origin, g_sys.flights[i].destination);
            printf("      Departure (出发): %s\n", g_sys.flights[i].departureTime);
            printf("      Available (余票): %d / %d\n",
                   g_sys.flights[i].totalSeats - g_sys.flights[i].bookedCount,
                   g_sys.flights[i].totalSeats);
        }
    }

    if (found == 0)
        printf("\nNo matching flights found. (未找到匹配航班)\n");
    else
        printf("\nFound %d matching record(s). (找到 %d 条匹配记录)\n", found, found);
    pauseScreen();
}

//Display Seat Map (显示座位图)
//[ ] = Available (空闲), [X] = Booked (已订), A / D are window seats (靠窗)
static void showSeatMap(const Flight *f){
    int r, c;
    printf("\n========== Seat Map (座位图) [%s] ==========\n", (*f).flightNo);
    printf("  Flight (航班): %s   %s --> %s\n",
           (*f).flightNo, (*f).origin, (*f).destination);
    printf("  Departure (出发): %s\n\n", (*f).departureTime);
    printf("        Window (窗)       Window (窗)\n");
    printf("          A      B      C      D\n");
    printf("      +-----+-----+-----+-----+\n");

    for (r = 0; r < (*f).totalRows; r++){
        printf("  %2d  ", r + 1);
        for (c = 0; c < SEATS_PER_ROW; c++){
            char mark = ((*f).seats[r][c].status == SEAT_BOOKED) ? 'X' : ' ';
            printf("| [%c] ", mark);
        }
        printf("|\n");
        if (r < (*f).totalRows - 1)
            printf("      +-----+-----+-----+-----+\n");
    }
    printf("      +-----+-----+-----+-----+\n");
    printf("  Legend (图例): [ ]=Free(空闲)  [X]=Booked(已订)  A/D=Window(靠窗)\n");
}

//Print Passenger List for a Flight (打印乘客清单)
static void printPassengerList(const Flight *f){
    int i;
    if ((*f).passengerCount == 0){
        printf("\nNo passengers on this flight. (该航班暂无乘客)\n");
        return;
    }

    printf("\n========== Passenger List (乘客清单) [%s] ==========\n", (*f).flightNo);
    printf("  Flight (航班): %s   %s --> %s\n\n",
           (*f).flightNo, (*f).origin, (*f).destination);
    printf("  %-6s %-20s %-20s %-8s\n",
           "No.(序号)", "Name(姓名)", "ID Card(身份证号)", "Seat(座位)");
    printf("  %-6s %-20s %-20s %-8s\n",
           "--------", "--------------------", "--------------------", "--------");

    for (i = 0; i < (*f).passengerCount; i++){
        printf("  %-6d %-20s %-20s %d%c",
               i + 1,
               (*f).passengers[i].name,
               (*f).passengers[i].id,
               (*f).passengers[i].seatRow,
               (*f).passengers[i].seatCol);
        if (isWindowSeat((*f).passengers[i].seatCol))
            printf(" (Window/靠窗)");
        printf("\n");
    }
    printf("\n  Total (共): %d passenger(s) (名乘客).\n", (*f).passengerCount);
}

//4. Book Ticket (Passenger) — (乘客订票)
//Select flight -> View seat map -> Choose seat ->Enter passenger info -> Confirm booking
//(选择航班 -> 查看座位图 -> 选择座位 -> 填写信息 -> 确认)
static void passengerBookTicket(void){
    char flightNo[MAX_FLIGHT_NO_LEN];
    int idx, row, r, c_idx, i, flIdx, pc;
    char col, confirm;

    if (g_sys.flightCount == 0){
        printf("\nNo flights available. Cannot book ticket.\n");
        printf("(暂无航班, 无法订票)\n");
        pauseScreen();
        return;
    }

    printf("\n========== Book Ticket (乘客订票) ==========\n");
    printf("\nAvailable flights (可选航班列表):\n");
    for (i = 0; i < g_sys.flightCount; i++){
        printf("  %s  %s --> %s  Departure(出发): %s  Available(余票): %d/%d\n",
               g_sys.flights[i].flightNo, g_sys.flights[i].origin, g_sys.flights[i].destination,
               g_sys.flights[i].departureTime,
               g_sys.flights[i].totalSeats - g_sys.flights[i].bookedCount,
               g_sys.flights[i].totalSeats);
    }

    printf("\nEnter flight number to book (请输入航班号): ");
    scanf("%s", flightNo);
    clearInputBuffer();

    idx = findFlightIndex(flightNo);
    if (idx == -1){
        printf("[Error] Flight '%s' not found. (未找到航班 '%s')\n", flightNo, flightNo);
        pauseScreen();
        return;
    }
    flIdx = idx;

    if (g_sys.flights[flIdx].bookedCount >= g_sys.flights[flIdx].totalSeats){
        printf("[Error] Flight '%s' is fully booked! (航班 '%s' 已满座!)\n", flightNo, flightNo);
        pauseScreen();
        return;
    }

    showSeatMap(&g_sys.flights[flIdx]);

    printf("\nEnter seat (请输入座位, format: row+column 排号+列号, e.g. 10D): ");
    scanf("%d%c", &row, &col);
    clearInputBuffer();

    if (row < 1 || row > g_sys.flights[flIdx].totalRows){
        printf("[Error] Row must be between 1 and %d! (排号必须在 1~%d 之间!)\n",
               g_sys.flights[flIdx].totalRows, g_sys.flights[flIdx].totalRows);
        pauseScreen();
        return;
    }
    if (col < 'A' || col > 'D'){
        printf("[Error] Column must be A/B/C/D! (列号必须为 A/B/C/D!)\n");
        pauseScreen();
        return;
    }

    r = row - 1;
    c_idx = (int)(col - 'A');

    if (g_sys.flights[flIdx].seats[r][c_idx].status == SEAT_BOOKED){
        printf("[Error] Seat %d%c is already booked! (座位 %d%c 已被预订!)\n", row, col, row, col);
        pauseScreen();
        return;
    }

    pc = g_sys.flights[flIdx].passengerCount;
    printf("Enter passenger name (请输入乘客姓名): ");
    scanf("%s", g_sys.flights[flIdx].passengers[pc].name);
    clearInputBuffer();
    printf("Enter ID card number (请输入身份证号): ");
    scanf("%s", g_sys.flights[flIdx].passengers[pc].id);
    clearInputBuffer();
    g_sys.flights[flIdx].passengers[pc].seatRow = row;
    g_sys.flights[flIdx].passengers[pc].seatCol = col;

    printf("\n=======================================\n");
    printf("  Booking Confirmation (订票确认):\n");
    printf("  Flight (航班): %s (%s --> %s)\n",
           g_sys.flights[flIdx].flightNo, g_sys.flights[flIdx].origin, g_sys.flights[flIdx].destination);
    printf("  Departure (出发): %s\n", g_sys.flights[flIdx].departureTime);
    printf("  Seat (座位): %d%c", row, col);
    if (isWindowSeat(col)) printf(" (Window/靠窗)");
    printf("\n");
    printf("  Name (姓名): %s\n", g_sys.flights[flIdx].passengers[pc].name);
    printf("  ID Card (身份证): %s\n", g_sys.flights[flIdx].passengers[pc].id);
    printf("=======================================\n");
    printf("Confirm booking? (确认订票?) (Y/N): ");
    scanf("%c", &confirm);
    clearInputBuffer();

    if (confirm == 'Y' || confirm == 'y'){
        g_sys.flights[flIdx].seats[r][c_idx].status = SEAT_BOOKED;
        g_sys.flights[flIdx].bookedCount++;
        g_sys.flights[flIdx].passengerCount++;
        printf("\n[OK] Booking successful! (订票成功!) Seat (座位): %d%c\n", row, col);
    }else{
        printf("\nBooking cancelled. (已取消订票)\n");
    }
    pauseScreen();
}

//5. Sell Ticket (At Counter) — (现场售票)
static void passengerSellTicket(void){
    char flightNo[MAX_FLIGHT_NO_LEN];
    int idx, row, r, c_idx, i, flIdx, pc;
    char col, confirm;

    if (g_sys.flightCount == 0){
        printf("\nNo flights available. Cannot sell ticket.\n");
        printf("(暂无航班, 无法售票)\n");
        pauseScreen();
        return;
    }

    printf("\n========== Sell Ticket (现场售票) ==========\n");
    printf("\nAvailable flights (可选航班列表):\n");
    for (i = 0; i < g_sys.flightCount; i++){
        printf("  %s  %s --> %s  Departure(出发): %s  Available(余票): %d/%d\n",
               g_sys.flights[i].flightNo, g_sys.flights[i].origin, g_sys.flights[i].destination,
               g_sys.flights[i].departureTime,
               g_sys.flights[i].totalSeats - g_sys.flights[i].bookedCount,
               g_sys.flights[i].totalSeats);
    }

    printf("\nEnter flight number (请输入航班号): ");
    scanf("%s", flightNo);
    clearInputBuffer();

    idx = findFlightIndex(flightNo);
    if (idx == -1){
        printf("[Error] Flight '%s' not found. (未找到航班 '%s')\n", flightNo, flightNo);
        pauseScreen();
        return;
    }
    flIdx = idx;

    if (g_sys.flights[flIdx].bookedCount >= g_sys.flights[flIdx].totalSeats){
        printf("[Error] Flight '%s' is sold out! (航班 '%s' 已售罄!)\n", flightNo, flightNo);
        pauseScreen();
        return;
    }

    showSeatMap(&g_sys.flights[flIdx]);

    printf("\nSelect seat (请选择座位, e.g. 10D): ");
    scanf("%d%c", &row, &col);
    clearInputBuffer();

    if (row < 1 || row > g_sys.flights[flIdx].totalRows){
        printf("[Error] Invalid row number! (无效的排号!)\n");
        pauseScreen();
        return;
    }
    if (col < 'A' || col > 'D'){
        printf("[Error] Invalid column! (无效的列号!)\n");
        pauseScreen();
        return;
    }

    r = row - 1;
    c_idx = (int)(col - 'A');

    if (g_sys.flights[flIdx].seats[r][c_idx].status == SEAT_BOOKED){
        printf("[Error] Seat %d%c is already sold! (座位 %d%c 已售出!)\n", row, col, row, col);
        pauseScreen();
        return;
    }

    pc = g_sys.flights[flIdx].passengerCount;
    printf("Enter passenger name (请输入购票人姓名): ");
    scanf("%s", g_sys.flights[flIdx].passengers[pc].name);
    clearInputBuffer();
    printf("Enter ID card number (请输入身份证号): ");
    scanf("%s", g_sys.flights[flIdx].passengers[pc].id);
    clearInputBuffer();
    g_sys.flights[flIdx].passengers[pc].seatRow = row;
    g_sys.flights[flIdx].passengers[pc].seatCol = col;

    printf("\n=======================================\n");
    printf("  Ticket Sale Confirmation (售票确认):\n");
    printf("  Flight (航班): %s (%s --> %s)\n",
           g_sys.flights[flIdx].flightNo, g_sys.flights[flIdx].origin, g_sys.flights[flIdx].destination);
    printf("  Seat (座位): %d%c", row, col);
    if (isWindowSeat(col)) printf(" (Window/靠窗)");
    printf("\n");
    printf("  Passenger (乘客): %s\n", g_sys.flights[flIdx].passengers[pc].name);
    printf("  ID Card (身份证): %s\n", g_sys.flights[flIdx].passengers[pc].id);
    printf("  Fare (票价): $280.00 (example/示例)\n");
    printf("=======================================\n");
    printf("Confirm sale? (确认出票?) (Y/N): ");
    scanf("%c", &confirm);
    clearInputBuffer();

    if (confirm == 'Y' || confirm == 'y'){
        g_sys.flights[flIdx].seats[r][c_idx].status = SEAT_BOOKED;
        g_sys.flights[flIdx].bookedCount++;
        g_sys.flights[flIdx].passengerCount++;
        printf("\n[OK] Ticket sold! (售票成功!) Seat (座位): %d%c\n", row, col);
        printf("    Have a nice flight! (祝旅途愉快!)\n");
    }else{
        printf("\nSale cancelled. (已取消售票)\n");
    }
    pauseScreen();
}

//6. Refund Ticket (Passenger) — (乘客退票)
static void passengerRefundTicket(void){
    char flightNo[MAX_FLIGHT_NO_LEN];
    char keyword[MAX_NAME_LEN];
    int idx, p_idx, r, c_col, i, flIdx;
    int seatRow, seatCol;
    char confirm;

    if (g_sys.flightCount == 0){
        printf("\nNo flights available. (暂无航班信息)\n");
        pauseScreen();
        return;
    }

    printf("\n========== Refund Ticket (退票) ==========\n");
    printf("Enter flight number (请输入航班号): ");
    scanf("%s", flightNo);
    clearInputBuffer();

    idx = findFlightIndex(flightNo);
    if (idx == -1){
        printf("[Error] Flight '%s' not found. (未找到航班 '%s')\n", flightNo, flightNo);
        pauseScreen();
        return;
    }
    flIdx = idx;

    if (g_sys.flights[flIdx].passengerCount == 0){
        printf("No passengers on this flight. Cannot refund.\n");
        printf("(该航班无乘客, 无法退票)\n");
        pauseScreen();
        return;
    }

    printPassengerList(&g_sys.flights[flIdx]);

    printf("\nEnter passenger name or ID card number to refund:\n");
    printf("(请输入要退票的乘客姓名或身份证号): ");
    scanf("%s", keyword);
    clearInputBuffer();

    p_idx = -1;
    for (i = 0; i < g_sys.flights[flIdx].passengerCount; i++){
        if (strcmp(g_sys.flights[flIdx].passengers[i].name, keyword) == 0 ||
            strcmp(g_sys.flights[flIdx].passengers[i].id, keyword) == 0){
            p_idx = i;
            break;
        }
    }

    if (p_idx == -1){
        printf("[Error] No passenger matching '%s' found. (未找到匹配乘客 '%s')\n", keyword, keyword);
        pauseScreen();
        return;
    }

    printf("\n=======================================\n");
    printf("  Refund Confirmation (退票确认):\n");
    printf("  Passenger (乘客): %s  ID (身份证): %s\n",
           g_sys.flights[flIdx].passengers[p_idx].name,
           g_sys.flights[flIdx].passengers[p_idx].id);
    printf("  Seat (座位): %d%c\n",
           g_sys.flights[flIdx].passengers[p_idx].seatRow,
           g_sys.flights[flIdx].passengers[p_idx].seatCol);
    printf("=======================================\n");
    printf("Confirm refund? (确认退票?) (Y/N): ");
    scanf("%c", &confirm);
    clearInputBuffer();

    if (confirm == 'Y' || confirm == 'y'){
        seatRow = g_sys.flights[flIdx].passengers[p_idx].seatRow;
        seatCol = g_sys.flights[flIdx].passengers[p_idx].seatCol;
        r = seatRow - 1;
        c_col = (int)(seatCol - 'A');
        g_sys.flights[flIdx].seats[r][c_col].status = SEAT_AVAILABLE;
        g_sys.flights[flIdx].bookedCount--;

        printf("\n[OK] Refund successful! (退票成功!)\n");
        printf("    Seat %d%c has been released. (座位 %d%c 已释放)\n",
               seatRow, seatCol, seatRow, seatCol);

        if (p_idx < g_sys.flights[flIdx].passengerCount - 1)
            g_sys.flights[flIdx].passengers[p_idx] =
                g_sys.flights[flIdx].passengers[g_sys.flights[flIdx].passengerCount - 1];
        g_sys.flights[flIdx].passengerCount--;
    }else{
        printf("\nRefund cancelled. (已取消退票)\n");
    }
    pauseScreen();
}

//7. Statistics — (统计信息)
static void passengerStatistics(void){
    int totalBooked = 0, totalSeats = 0, totalPassengers = 0;
    float overallRate;
    int i;

    printf("\n========== Statistics (统计信息) ==========\n");
    if (g_sys.flightCount == 0){
        printf("\nNo flights available. (暂无航班信息)\n");
        pauseScreen();
        return;
    }

    printf("\n%-10s %-10s %-10s %-10s %-8s %-8s\n",
           "Flight(航班)","Total(总数)","Booked(已订)","Remain(剩余)","Occup.(率)","Pass.(客)");
    printf("%-10s %-10s %-10s %-10s %-8s %-8s\n",
           "----------", "----------", "----------", "----------", "--------", "--------");

    for (i = 0; i < g_sys.flightCount; i++){
        int remain = g_sys.flights[i].totalSeats - g_sys.flights[i].bookedCount;
        float rate = (g_sys.flights[i].totalSeats > 0)
                         ? (float)g_sys.flights[i].bookedCount / g_sys.flights[i].totalSeats * 100.0f
                         : 0.0f;
        printf("%-10s %-10d %-10d %-10d %7.1f%% %-8d\n",
               g_sys.flights[i].flightNo, g_sys.flights[i].totalSeats, g_sys.flights[i].bookedCount,
               remain, rate, g_sys.flights[i].passengerCount);
        totalBooked     += g_sys.flights[i].bookedCount;
        totalSeats      += g_sys.flights[i].totalSeats;
        totalPassengers += g_sys.flights[i].passengerCount;
    }

    overallRate = (totalSeats > 0) ? (float)totalBooked / totalSeats * 100.0f : 0.0f;
    printf("\n------------------------------------------\n");
    printf("  Total Flights   (航班总数):    %d\n", g_sys.flightCount);
    printf("  Total Seats     (总座位数):    %d\n", totalSeats);
    printf("  Booked Seats    (已订座位):    %d\n", totalBooked);
    printf("  Available Seats (剩余座位):    %d\n", totalSeats - totalBooked);
    printf("  Total Passengers(总乘客数):    %d\n", totalPassengers);
    printf("  Overall Occupancy(整体上座率): %.1f%%\n", overallRate);
    pauseScreen();
}

//8. Query (Passenger) — (乘客查询)
//View flight details, seat map, passenger list, or search(查看航班详情、座位图、乘客清单、搜索)
//NOTE: Viewing passenger list requires admin password(注意: 查看乘客清单需要管理员密码)
static void passengerQuery(void){
    int choice, idx, flIdx;
    char flightNo[MAX_FLIGHT_NO_LEN];

    if (g_sys.flightCount == 0){
        printf("\nNo flights available. (暂无航班信息)\n");
        pauseScreen();
        return;
    }

    printf("\n========== Query (查询功能) ==========\n");
    printf("  1. View Flight Details (with Seat Map) (航班详情/座位图)\n");
    printf("  2. View Passenger List               (查看乘客清单)\n");
    printf("  3. Search Flights by Origin/Dest     (按起始地/目的地搜索)\n");
    printf("  0. Return (返回)\n");
    printf("Select (请选择) (0-3): ");
    scanf("%d", &choice);
    clearInputBuffer();

    if (choice == 0) return;

    if (choice == 3){
        adminSearchFlights();
        return;
    }

    printf("Enter flight number (请输入航班号): ");
    scanf("%s", flightNo);
    clearInputBuffer();

    idx = findFlightIndex(flightNo);
    if (idx == -1){
        printf("[Error] Flight '%s' not found. (未找到航班 '%s')\n", flightNo, flightNo);
        pauseScreen();
        return;
    }
    flIdx = idx;

    switch (choice){
    case 1:
        printf("\n---------- Flight Details (航班详情) ----------\n");
        printf("  Flight No:    (航班号) %s\n", g_sys.flights[flIdx].flightNo);
        printf("  Origin:       (起始地) %s\n", g_sys.flights[flIdx].origin);
        printf("  Destination:  (目的地) %s\n", g_sys.flights[flIdx].destination);
        printf("  Departure:    (出发)   %s\n", g_sys.flights[flIdx].departureTime);
        printf("  Total Seats:  (总座位) %d (%d rows/排, 4 seats/座 per row/排)\n",
               g_sys.flights[flIdx].totalSeats, g_sys.flights[flIdx].totalRows);
        printf("  Booked: %d (已订)  |  Available: %d (剩余)\n",
               g_sys.flights[flIdx].bookedCount,
               g_sys.flights[flIdx].totalSeats - g_sys.flights[flIdx].bookedCount);
        showSeatMap(&g_sys.flights[flIdx]);
        break;

    case 2:
        //View passenger list - requires admin password (查看乘客清单 - 需要管理员密码)
        {
            char pwd[20];
            printf("\nThis action requires admin authorization.\n");
            printf("(此操作需要管理员授权, 保护乘客信息安全)\n");
            printf("Enter admin password (请输入管理员密码): ");
            scanf("%s", pwd);
            clearInputBuffer();
            if (strcmp(pwd, "admin123") == 0)
                printPassengerList(&g_sys.flights[flIdx]);
            else
                printf("[Error] Wrong password! Access denied. (错误) 密码错误! 访问被拒绝.\n");
        }
        break;

    default:
        printf("[Error] Invalid option. (无效选项)\n");
        break;
    }
    pauseScreen();
}
