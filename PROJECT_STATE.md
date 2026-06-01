# PROJECT STATE / 项目状态

## 1. Implemented Features / 已实现功能

| # | Feature / 功能 | Role / 角色 | Status |
|---|---------------|-------------|--------|
| 1 | Add Flight Info / 航班信息录入 | Admin | Done |
| 2 | Browse All Flights / 航班信息浏览 | Admin | Done |
| 3 | Search Flights / 航班信息搜索 | Admin + Passenger | Done |
| 4 | Book Ticket / 乘客订票 | Passenger | Done |
| 5 | Sell Ticket / 售票 | Passenger | Done |
| 6 | Refund Ticket / 退票 | Passenger | Done |
| 7 | Statistics / 统计 | Passenger | Done |
| 8 | Query / 查询 | Passenger | Done (passenger list requires admin password) |

### Feature Details / 功能详情

- Flight search by: flight number, origin city, destination city / 可按航班号、起始地、目的地搜索
- Seat selection: visual seat map, `[X]`=Booked `[ ]`=Free, A/D=Window / 可视化座位图选座
- Passenger list viewing: password-protected (`admin123`) / 乘客清单需管理员密码
- Statistics: per-flight occupancy rate + overall summary / 单航班上座率 + 全局汇总

## 2. Data Structure Design / 数据结构设计

```
FlightSystem g_sys (global)
  +-- Flight flights[MAX_FLIGHTS]
        +-- flightNo[10]       (flight number / 航班号)
        +-- origin[20]         (origin city / 起始地)
        +-- destination[20]    (destination city / 目的地)
        +-- departureTime[20]  (departure time / 出发时间)
        +-- totalRows          (seat rows / 总排数)
        +-- totalSeats         (total seats = rows * 4 / 总座位数)
        +-- bookedCount        (booked seats / 已订数)
        +-- Seat seats[50][4]  (seat matrix / 座位矩阵)
        +-- Passenger passengers[200] (passenger list / 乘客列表)
        +-- passengerCount     (current passenger count / 当前乘客数)

Seat
  +-- row    (row number 1~50 / 排号)
  +-- col    (column 'A'|'B'|'C'|'D' / 列号)
  +-- status (SEAT_AVAILABLE=0 / SEAT_BOOKED=1)

Passenger
  +-- name[20]    (passenger name / 乘客姓名)
  +-- id[18]      (ID card number / 身份证号)
  +-- seatRow     (assigned seat row / 座位排号)
  +-- seatCol     (assigned seat column / 座位列号)

Enum:
  - SeatStatus: { SEAT_AVAILABLE, SEAT_BOOKED }
  - UserRole:   { ROLE_ADMIN, ROLE_PASSENGER } (reserved)
```

## 3. Flight Struct Definition / Flight 结构体定义

```c
typedef struct {
    char   flightNo[MAX_FLIGHT_NO_LEN];       // Flight number / 航班号
    char   origin[MAX_CITY_LEN];              // Origin city / 起始地
    char   destination[MAX_CITY_LEN];         // Destination city / 目的地
    char   departureTime[MAX_TIME_LEN];       // Departure time / 出发时间
    int    totalRows;                         // Total seat rows / 总排数
    int    totalSeats;                        // Total seats = totalRows * 4 / 总座位数
    int    bookedCount;                       // Booked seat count / 已订座位数
    Seat   seats[MAX_ROWS][SEATS_PER_ROW];    // Seat matrix / 座位矩阵
    Passenger passengers[MAX_PASSENGERS];     // Passenger list / 乘客列表
    int    passengerCount;                    // Current passenger count / 当前乘客数
} Flight;
```

MAX_FLIGHTS=10, MAX_PASSENGERS=200, MAX_ROWS=50, SEATS_PER_ROW=4

## 4. Ticket Struct / Ticket 结构体定义

**NOT IMPLEMENTED / 未实现.**

Current design embeds `Passenger` directly inside `Flight`. Passengers are stored as an array `passengers[MAX_PASSENGERS]` within each flight. There is no separate `Ticket` struct. A "ticket" is implicitly represented by a `Passenger` entry with its assigned `seatRow` and `seatCol`.

If a Ticket struct is needed, suggested design:

```c
typedef struct {
    char ticketId[20];          // Ticket ID / 票号
    char flightNo[10];          // Flight number / 航班号
    char passengerName[20];     // Name / 姓名
    char passengerId[18];       // ID card / 身份证号
    int  seatRow;               // Seat row / 座位排号
    char seatCol;               // Seat column / 座位列号
    char status[10];            // "booked"/"refunded"/"used"
    char bookingTime[20];       // Booking time / 订票时间
    float fare;                 // Fare / 票价
} Ticket;
```

## 5. File Storage Format / 文件存储格式

**DONE / 已完成.** (2026-06-01)

File persistence implemented using text format in `flights.dat`:

```
Format:
  Line 1: <flightCount>
  For each flight:  F <flightNo> <origin> <destination> <departureTime> <totalRows>
  For each passenger: P <name> <id> <seatRow> <seatCol>

Example:
  2
  F CA1234 Beijing Shanghai 2026-06-01_08:00 10
  P Alice 110101199001010001 1 A
  P Bob   110101199002020002 1 B
  F MU5678 Shanghai Guangzhou 2026-06-02_14:30 8
```

Functions added:
- `saveSystem()` — saves all flights and passengers to flights.dat (auto-called on exit)
- `loadSystem()` — loads data from flights.dat on startup (if file exists)
- Admin menu option 4: manual save to file

## 6. Remaining TODO / 当前未完成任务

| # | Task / 任务 | Priority | Notes |
|---|------------|----------|-------|
| 1 | Ticket struct / Ticket 结构体 | Medium | Separate ticket from passenger info |
| 2 | File persistence / 文件持久化存储 | Done (2026-06-01) | Save/load via flights.dat text file, auto-save on exit |
| 3 | Multiple tickets per passenger / 一人多票 | Low | Currently 1 passenger = 1 seat = 1 ticket |
| 4 | Fare system / 票价系统 | Low | Currently hardcoded $280.00 example |
| 5 | Report document / 课程设计报告 | Done (2026-06-01) | Generated: 飞机航班订票系统_课程设计报告.docx |
| 6 | Delete flight / 删除航班 | Low | Admin cannot remove a flight |
| 7 | Modify flight / 修改航班信息 | Low | Admin cannot edit existing flight info |
| 8 | Duplicate passenger check / 重复乘客检查 | Medium | Same ID card can book multiple seats |
| 9 | Seat class / 舱位等级 | Low | Economy/business/first class |

### File List / 文件清单

| File | Description |
|------|-------------|
| main.c | Main source code / 主源代码 (~1000 lines, C89/GBK) |
| flights.dat | Persistent data file / 持久化数据文件 (auto-generated) |
| 飞机航班订票系统_课程设计报告.docx | Course design report / 课程设计报告 |
| README.md | Project documentation / 项目说明 |
| PROJECT_STATE.md | This file / 本文件 |
| .gitignore | Git ignore rules |
| *.vcxproj | VC++2010 project files |
