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

//main() - Program entry point (程序入口)
int main(void){
    int choice;
    //Initialize system (初始化系统)
    g_sys.flightCount = 0;
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
        printf("|  0. Return to Main Menu(返回主菜单)    |\n");
        printf("+--------------------------------------+\n");
        printf("Please select (请选择) (0-3): ");
        scanf("%d", &choice);
        clearInputBuffer();

        switch (choice){
        case 0: return;
        case 1: adminAddFlight();      break;
        case 2: adminBrowseFlights();  break;
        case 3: adminSearchFlights();  break;
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

//Find a flight's index in the array by its flight number.(根据航班号在数组中查找航班索引)
//@param flightNo  The flight number to search for. (要查找的航班号)
//@return Index (0 ~ flightCount-1), or -1 if not found.(返回下标, 未找到返回 -1)
static int findFlightIndex(const char *flightNo){
    int i;
    for (i = 0; i < g_sys.flightCount; i++){
        if (strcmp(g_sys.flights[i].flightNo, flightNo) == 0){
            return i;
        }
    }
    return -1;
}

//Check whether a seat column is a window seat.(判断是否为靠窗座位)
//Seat layout: A(window) B(aisle) C(aisle) D(window)(座位排列: A(窗) B(过道) C(过道) D(窗))
static int isWindowSeat(char col){
    return (col == 'A' || col == 'D');
}

//Initialize the seat matrix for a flight.(初始化航班座位矩阵)
//@param f    Pointer to the flight. (航班指针)
//@param rows Number of seat rows.   (座位排数)
static void initSeats(Flight *f, int rows){
    int r, c;

    f->totalRows  = rows;
    f->totalSeats = rows * SEATS_PER_ROW;
    f->bookedCount = 0;
    f->passengerCount = 0;

    for (r = 0; r < rows; r++){
        for (c = 0; c < SEATS_PER_ROW; c++){
            f->seats[r][c].row    = r + 1;				//Row starts from 1 (排号从1开始)
            f->seats[r][c].col    = (char)('A' + c);	//A, B, C, D
            f->seats[r][c].status = SEAT_AVAILABLE;
        }
    }
}

//1. Add Flight Info (Admin) — (航班信息录入)
static void adminAddFlight(void){
    Flight *f;
    int rows;

    if (g_sys.flightCount >= MAX_FLIGHTS){
        printf("\n[Error] Maximum number of flights (%d) reached.\n",
               MAX_FLIGHTS);
        printf("(错误) 航班数量已达上限 (%d)。\n", MAX_FLIGHTS);
        pauseScreen();
        return;
    }

    f = &g_sys.flights[g_sys.flightCount];

    printf("\n========== Add Flight Info (航班信息录入) ==========\n");

    printf("Enter flight number (输入航班号, e.g. CA1234): ");
    scanf("%s", f->flightNo);
    clearInputBuffer();

    //Check for duplicate flight number (检查航班号是否重复)
    if (findFlightIndex(f->flightNo) != -1){
        printf("[Error] Flight '%s' already exists! (航班 '%s' 已存在!)\n",
               f->flightNo, f->flightNo);
        return;
    }

    printf("Enter origin city (输入起始地, e.g. Beijing): ");
    scanf("%s", f->origin);
    clearInputBuffer();

    printf("Enter destination city (输入目的地, e.g. Shanghai): ");
    scanf("%s", f->destination);
    clearInputBuffer();

    printf("Enter departure time (输入出发时间, e.g. 2026-06-01 08:00): ");
    scanf("%[^\n]", f->departureTime);
    clearInputBuffer();

    printf("Enter number of seat rows (输入座位排数) (1-%d, 4 seats/row A/B/C/D): ",
           MAX_ROWS);
    scanf("%d", &rows);
    clearInputBuffer();

    if (rows < 1 || rows > MAX_ROWS){
        printf("[Error] Rows must be between 1 and %d! (排数必须在 1~%d 之间!)\n",
               MAX_ROWS, MAX_ROWS);
        return;
    }

    initSeats(f, rows);
    g_sys.flightCount++;

    printf("\n[OK] Flight '%s' added! (航班 '%s' 录入成功!)\n",
           f->flightNo, f->flightNo);
    printf("     %d seats total (%d rows x 4). (共 %d 座, %d 排 x 4)\n",
           f->totalSeats, f->totalRows, f->totalSeats, f->totalRows);
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
        Flight *f;
        f = &g_sys.flights[i];

        printf("\n--------------------------------------\n");
        printf("  Flight No:    (航班号) %s\n", f->flightNo);
        printf("  Origin:       (起始地) %s\n", f->origin);
        printf("  Destination:  (目的地) %s\n", f->destination);
        printf("  Departure:    (出发时间) %s\n", f->departureTime);
        printf("  Total Seats:  (总座位) %d rows x 4 = %d\n",
               f->totalRows, f->totalSeats);
        printf("  Booked: %d (已订)  |  Available: %d (剩余)\n",
               f->bookedCount, f->totalSeats - f->bookedCount);
        printf("  Passengers:   (乘客数) %d\n", f->passengerCount);
    }

    pauseScreen();
}

//Search Flights (Admin & Passenger shared) — (航班信息搜索)
//Search by flight number / origin / destination(可按航班号、起始地、目的地搜索)
static void adminSearchFlights(void){
    int choice;
    char keyword[MAX_CITY_LEN];
    int found;
    int i;

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
        Flight *f;
        int match;

        f = &g_sys.flights[i];
        match = 0;

        switch (choice){
        case 1:
            match = (strcmp(f->flightNo, keyword) == 0);
            break;
        case 2:
            match = (strcmp(f->origin, keyword) == 0);
            break;
        case 3:
            match = (strcmp(f->destination, keyword) == 0);
            break;
        default:
            printf("[Error] Invalid search mode. (无效的搜索方式)\n");
            return;
        }

        if (match){
            found++;
            printf("\n  [%d] Flight (航班): %s\n", found, f->flightNo);
            printf("      Route (航线): %s --> %s\n",
                   f->origin, f->destination);
            printf("      Departure (出发): %s\n", f->departureTime);
            printf("      Available (余票): %d / %d\n",
                   f->totalSeats - f->bookedCount, f->totalSeats);
        }
    }

    if (found == 0){
        printf("\nNo matching flights found. (未找到匹配航班)\n");
    }
		else{
        printf("\nFound %d matching record(s). (找到 %d 条匹配记录)\n",
               found, found);
		}
    pauseScreen();
}

//Display Seat Map (显示座位图)
//[ ] = Available (空闲), [X] = Booked (已订),A / D are window seats (靠窗)
static void showSeatMap(const Flight *f){
    int r, c;

    printf("\n========== Seat Map (座位图) [%s] ==========\n", f->flightNo);
    printf("  Flight (航班): %s   %s --> %s\n",
           f->flightNo, f->origin, f->destination);
    printf("  Departure (出发): %s\n\n", f->departureTime);
    printf("Window (窗)                    Window (窗)\n");
    printf("         A     B     C     D\n");
    printf("      +-----+-----+-----+-----+\n");

    for (r = 0; r < f->totalRows; r++){
        printf("  %2d  ", r + 1);  /* Row number (排号) */

        for (c = 0; c < SEATS_PER_ROW; c++){
            char mark;
            mark = (f->seats[r][c].status == SEAT_BOOKED) ? 'X' : ' ';
            printf("| [%c] ", mark);
        }
        printf("|\n");

        if (r < f->totalRows - 1){
            printf("      +-----+-----+-----+-----+\n");
        }
    }

    printf("      +-----+-----+-----+-----+\n");
    printf("  Legend (图例): [ ]=Free(空闲)  [X]=Booked(已订)  A/D=Window(靠窗)\n");
}

//Print Passenger List for a Flight (打印乘客清单)
static void printPassengerList(const Flight *f){
    int i;

    if (f->passengerCount == 0){
        printf("\nNo passengers on this flight. (该航班暂无乘客)\n");
        return;
    }

    printf("\n========== Passenger List (乘客清单) [%s] ==========\n",
           f->flightNo);
    printf("  Flight (航班): %s   %s --> %s\n\n",
           f->flightNo, f->origin, f->destination);
    printf("  %-6s %-20s %-20s %-8s\n",
           "No.(序号)", "Name(姓名)", "ID Card(身份证号)", "Seat(座位)");
    printf("  %-6s %-20s %-20s %-8s\n",
           "--------", "--------------------", "--------------------", "--------");

    for (i = 0; i < f->passengerCount; i++){
        const Passenger *p;
        p = &f->passengers[i];

        printf("  %-6d %-20s %-20s %d%c",
               i + 1,
               p->name,
               p->id,
               p->seatRow,
               p->seatCol);

        if (isWindowSeat(p->seatCol)){
            printf(" (Window/靠窗)");
        }
        printf("\n");
    }

    printf("\n  Total (共): %d passenger(s) (名乘客).\n", f->passengerCount);
}

//4. Book Ticket (Passenger) — (乘客订票)
//Select flight -> View seat map -> Choose seat ->Enter passenger info -> Confirm booking(选择航班 -> 查看座位图 -> 选择座位 -> 填写信息 -> 确认)
static void passengerBookTicket(void){
    char flightNo[MAX_FLIGHT_NO_LEN];
    int idx;
    int row;
    char col;
    int r, c_idx;
    char confirm;
    Flight *f;
    Passenger *p;
    int i;

    if (g_sys.flightCount == 0){
        printf("\nNo flights available. Cannot book ticket.\n");
        printf("(暂无航班, 无法订票)\n");
        pauseScreen();
        return;
    }

    printf("\n========== Book Ticket (乘客订票) ==========\n");

    //List all available flights (列出可选航班)
    printf("\nAvailable flights (可选航班列表):\n");
    for (i = 0; i < g_sys.flightCount; i++){
        Flight *fl;
        fl = &g_sys.flights[i];
        printf("  %s  %s --> %s  Departure(出发): %s  Available(余票): %d/%d\n",
               fl->flightNo, fl->origin, fl->destination,
               fl->departureTime,
               fl->totalSeats - fl->bookedCount,
               fl->totalSeats);
    }

    printf("\nEnter flight number to book (请输入航班号): ");
    scanf("%s", flightNo);
    clearInputBuffer();

    idx = findFlightIndex(flightNo);
    if (idx == -1){
        printf("[Error] Flight '%s' not found. (未找到航班 '%s')\n",
               flightNo, flightNo);
        pauseScreen();
        return;
    }

    f = &g_sys.flights[idx];

    //Check if seats are available (检查是否满座)
    if (f->bookedCount >= f->totalSeats){
        printf("[Error] Flight '%s' is fully booked! (航班 '%s' 已满座!)\n",
               flightNo, flightNo);
        pauseScreen();
        return;
    }

    //Display seat map (显示座位图)
    showSeatMap(f);

    //Choose seat (选择座位)
    printf("\nEnter seat (请输入座位, format: row+column 排号+列号, e.g. 10D): ");
    scanf("%d%c", &row, &col);
    clearInputBuffer();

    //Validate seat (验证座位)
    if (row < 1 || row > f->totalRows){
        printf("[Error] Row must be between 1 and %d! (排号必须在 1~%d 之间!)\n",
               f->totalRows, f->totalRows);
        pauseScreen();
        return;
    }

    if (col < 'A' || col > 'D'){
        printf("[Error] Column must be A/B/C/D! (列号必须为 A/B/C/D!)\n");
        pauseScreen();
        return;
    }

    r = row - 1;	//Convert to 0-based index (转为0基下标)
    c_idx = (int)(col - 'A');

    if (f->seats[r][c_idx].status == SEAT_BOOKED){
        printf("[Error] Seat %d%c is already booked! (座位 %d%c 已被预订!)\n",
               row, col, row, col);
        pauseScreen();
        return;
    }

    //Enter passenger info (填写乘客信息)
    p = &f->passengers[f->passengerCount];

    printf("Enter passenger name (请输入乘客姓名): ");
    scanf("%s", p->name);
    clearInputBuffer();

    printf("Enter ID card number (请输入身份证号): ");
    scanf("%s", p->id);
    clearInputBuffer();

    p->seatRow = row;
    p->seatCol = col;

    //Confirm booking (确认订票)
    printf("\n=======================================\n");
    printf("  Booking Confirmation (订票确认):\n");
    printf("  Flight (航班): %s (%s --> %s)\n",
           f->flightNo, f->origin, f->destination);
    printf("  Departure (出发): %s\n", f->departureTime);
    printf("  Seat (座位): %d%c", row, col);
    if (isWindowSeat(col)) printf(" (Window/靠窗)");
    printf("\n");
    printf("  Name (姓名): %s\n", p->name);
    printf("  ID Card (身份证): %s\n", p->id);
    printf("=======================================\n");

    printf("Confirm booking? (确认订票?) (Y/N): ");
    scanf("%c", &confirm);
    clearInputBuffer();

    if (confirm == 'Y' || confirm == 'y'){
        f->seats[r][c_idx].status = SEAT_BOOKED;
        f->bookedCount++;
        f->passengerCount++;
        printf("\n[OK] Booking successful! (订票成功!) Seat (座位): %d%c\n",
               row, col);
    }
    else{
        printf("\nBooking cancelled. (已取消订票)\n");
    }

    pauseScreen();
}


//5. Sell Ticket (At Counter) — (现场售票)
//Similar to booking, with payment info added(与订票流程类似, 增加票价信息)
static void passengerSellTicket(void){
    char flightNo[MAX_FLIGHT_NO_LEN];
    int idx;
    int row;
    char col;
    int r, c_idx;
    char confirm;
    Flight *f;
    Passenger *p;
    int i;

    if (g_sys.flightCount == 0){
        printf("\nNo flights available. Cannot sell ticket.\n");
        printf("(暂无航班, 无法售票)\n");
        pauseScreen();
        return;
    }

    printf("\n========== Sell Ticket (现场售票) ==========\n");

    printf("\nAvailable flights (可选航班列表):\n");
    for (i = 0; i < g_sys.flightCount; i++){
        Flight *fl;
        fl = &g_sys.flights[i];
        printf("  %s  %s --> %s  Departure(出发): %s  Available(余票): %d/%d\n",
               fl->flightNo, fl->origin, fl->destination,
               fl->departureTime,
               fl->totalSeats - fl->bookedCount,
               fl->totalSeats);
    }

    printf("\nEnter flight number (请输入航班号): ");
    scanf("%s", flightNo);
    clearInputBuffer();

    idx = findFlightIndex(flightNo);
    if (idx == -1){
        printf("[Error] Flight '%s' not found. (未找到航班 '%s')\n",
               flightNo, flightNo);
        pauseScreen();
        return;
    }

    f = &g_sys.flights[idx];

    if (f->bookedCount >= f->totalSeats){
        printf("[Error] Flight '%s' is sold out! (航班 '%s' 已售罄!)\n",
               flightNo, flightNo);
        pauseScreen();
        return;
    }

    showSeatMap(f);

    printf("\nSelect seat (请选择座位, e.g. 10D): ");
    scanf("%d%c", &row, &col);
    clearInputBuffer();

    if (row < 1 || row > f->totalRows){
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

    if (f->seats[r][c_idx].status == SEAT_BOOKED){
        printf("[Error] Seat %d%c is already sold! (座位 %d%c 已售出!)\n",
               row, col, row, col);
        pauseScreen();
        return;
    }

    p = &f->passengers[f->passengerCount];

    printf("Enter passenger name (请输入购票人姓名): ");
    scanf("%s", p->name);
    clearInputBuffer();

    printf("Enter ID card number (请输入身份证号): ");
    scanf("%s", p->id);
    clearInputBuffer();

    p->seatRow = row;
    p->seatCol = col;

    //Confirm sale (确认出售)
    printf("\n=======================================\n");
    printf("  Ticket Sale Confirmation (售票确认):\n");
    printf("  Flight (航班): %s (%s --> %s)\n",
           f->flightNo, f->origin, f->destination);
    printf("  Seat (座位): %d%c", row, col);
    if (isWindowSeat(col)) printf(" (Window/靠窗)");
    printf("\n");
    printf("  Passenger (乘客): %s\n", p->name);
    printf("  ID Card (身份证): %s\n", p->id);
    printf("  Fare (票价): $280.00 (example/示例)\n");
    printf("=======================================\n");

    printf("Confirm sale? (确认出票?) (Y/N): ");
    scanf("%c", &confirm);
    clearInputBuffer();

    if (confirm == 'Y' || confirm == 'y'){
        f->seats[r][c_idx].status = SEAT_BOOKED;
        f->bookedCount++;
        f->passengerCount++;
        printf("\n[OK] Ticket sold! (售票成功!) Seat (座位): %d%c\n", row, col);
        printf("    Have a nice flight! (祝旅途愉快!)\n");
    }
    else{
        printf("\nSale cancelled. (已取消售票)\n");
    }

    pauseScreen();
}

//6. Refund Ticket (Passenger) — (乘客退票)
//Find passenger by name or ID number and cancel the ticket(根据姓名或身份证号查找乘客并退票)
static void passengerRefundTicket(void){
    char flightNo[MAX_FLIGHT_NO_LEN];
    char keyword[MAX_NAME_LEN];   //Name or ID card number (姓名或身份证号)
    int idx;
    int p_idx;
    char confirm;
    Flight *f;
    Passenger *p;
    int r, c_col;
    int i;

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
        printf("[Error] Flight '%s' not found. (未找到航班 '%s')\n",
               flightNo, flightNo);
        pauseScreen();
        return;
    }

    f = &g_sys.flights[idx];

    if (f->passengerCount == 0){
        printf("No passengers on this flight. Cannot refund.\n");
        printf("(该航班无乘客, 无法退票)\n");
        pauseScreen();
        return;
    }

	//List current passengers (列出当前乘客)
    printPassengerList(f);

    printf("\nEnter passenger name or ID card number to refund:\n");
    printf("(请输入要退票的乘客姓名或身份证号): ");
    scanf("%s", keyword);
    clearInputBuffer();

    //Search for passenger (查找乘客)
    p_idx = -1;
    for (i = 0; i < f->passengerCount; i++){
        if (strcmp(f->passengers[i].name, keyword) == 0 ||strcmp(f->passengers[i].id, keyword) == 0){
            p_idx = i;
            break;
        }
    }

    if (p_idx == -1){
        printf("[Error] No passenger matching '%s' found. (未找到匹配乘客 '%s')\n",
               keyword, keyword);
        pauseScreen();
        return;
    }

    p = &f->passengers[p_idx];

    //Confirm refund (确认退票)
    printf("\n=======================================\n");
    printf("  Refund Confirmation (退票确认):\n");
    printf("  Passenger (乘客): %s  ID (身份证): %s\n", p->name, p->id);
    printf("  Seat (座位): %d%c\n", p->seatRow, p->seatCol);
    printf("=======================================\n");

    printf("Confirm refund? (确认退票?) (Y/N): ");
    scanf("%c", &confirm);
    clearInputBuffer();

    if (confirm == 'Y' || confirm == 'y'){
        //Release the seat (释放座位)
        r = p->seatRow - 1;
        c_col = (int)(p->seatCol - 'A');
        f->seats[r][c_col].status = SEAT_AVAILABLE;
        f->bookedCount--;

        //Remove passenger from list (从乘客列表移除)
        if (p_idx < f->passengerCount - 1){
            f->passengers[p_idx] = f->passengers[f->passengerCount - 1];
        }
        f->passengerCount--;

        printf("\n[OK] Refund successful! (退票成功!)\n");
        printf("    Seat %d%c has been released. (座位 %d%c 已释放)\n",p->seatRow, p->seatCol, p->seatRow, p->seatCol);
    }
    else{
        printf("\nRefund cancelled. (已取消退票)\n");
    }

    pauseScreen();
}


//7. Statistics — (统计信息)
//Show occupancy rate for all flights (各航班上座率统计)
static void passengerStatistics(void){
    int totalBooked     = 0;
    int totalSeats      = 0;
    int totalPassengers = 0;
    float overallRate;
    int i;

    printf("\n========== Statistics (统计信息) ==========\n");

    if (g_sys.flightCount == 0){
        printf("\nNo flights available. (暂无航班信息)\n");
        pauseScreen();
        return;
    }

    printf("\n%-10s %-10s %-10s %-10s %-8s %-8s\n","Flight(航班)","Total(总数)","Booked(已订)","Remain(剩余)","Occup.(率)","Pass.(客)");
    printf("%-10s %-10s %-10s %-10s %-8s %-8s\n","----------", "----------", "----------","----------", "--------", "--------");

    for (i = 0; i < g_sys.flightCount; i++){
        Flight *f;
        int remain;
        float rate;

        f = &g_sys.flights[i];
        remain = f->totalSeats - f->bookedCount;
        rate = (f->totalSeats > 0)
                   ? (float)f->bookedCount / f->totalSeats * 100.0f
                   : 0.0f;

        printf("%-10s %-10d %-10d %-10d %7.1f%% %-8d\n",f->flightNo, f->totalSeats, f->bookedCount,remain, rate, f->passengerCount);

        totalBooked     += f->bookedCount;
        totalSeats      += f->totalSeats;
        totalPassengers += f->passengerCount;
    }

    overallRate = (totalSeats > 0)
                      ? (float)totalBooked / totalSeats * 100.0f
                      : 0.0f;

    printf("\n------------------------------------------\n");
    printf("  Total Flights   (航班总数):    %d\n", g_sys.flightCount);
    printf("  Total Seats     (总座位数):    %d\n", totalSeats);
    printf("  Booked Seats    (已订座位):    %d\n", totalBooked);
    printf("  Available Seats (剩余座位):    %d\n", totalSeats - totalBooked);
    printf("  Total Passengers(总乘客数):    %d\n", totalPassengers);
    printf("  Overall Occupancy(整体上座率): %.1f%%\n", overallRate);

    pauseScreen();
}

//Query (Passenger) — (乘客查询)
//View flight details, seat map, passenger list, or search(查看航班详情、座位图、乘客清单、搜索)
static void passengerQuery(void){
    int choice;
    char flightNo[MAX_FLIGHT_NO_LEN];
    int idx;
    Flight *f;

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

    if (choice == 0) 
		return;

    if (choice == 3){
        //Reuse the admin search function (复用管理员搜索功能)
        adminSearchFlights();
        return;
    }

    printf("Enter flight number (请输入航班号): ");
    scanf("%s", flightNo);
    clearInputBuffer();

    idx = findFlightIndex(flightNo);
    if (idx == -1){
        printf("[Error] Flight '%s' not found. (未找到航班 '%s')\n",flightNo, flightNo);
        pauseScreen();
        return;
    }

    f = &g_sys.flights[idx];

    switch (choice){
    case 1:
        //Show flight details + seat map (显示详情+座位图)
        printf("\n---------- Flight Details (航班详情) ----------\n");
        printf("  Flight No:    (航班号) %s\n", f->flightNo);
        printf("  Origin:       (起始地) %s\n", f->origin);
        printf("  Destination:  (目的地) %s\n", f->destination);
        printf("  Departure:    (出发)   %s\n", f->departureTime);
        printf("  Total Seats:  (总座位) %d (%d rows/排, 4 seats/座 per row/排)\n",
               f->totalSeats, f->totalRows);
        printf("  Booked: %d (已订)  |  Available: %d (剩余)\n",
               f->bookedCount, f->totalSeats - f->bookedCount);
        showSeatMap(f);
        break;

    case 2:{
        //View passenger list - requires admin password (查看乘客清单 - 需要管理员密码)
        char pwd[20];
        printf("\nThis action requires admin authorization.\n");
        printf("(此操作需要管理员授权, 保护乘客信息安全)\n");
        printf("Enter admin password (请输入管理员密码): ");
        scanf("%s", pwd);
        clearInputBuffer();
        if (strcmp(pwd, "admin123") == 0)
            printPassengerList(f);
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