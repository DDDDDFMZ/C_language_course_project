# Airplane Flight Booking System / 飞机航班订票系统

Compatible with VC++2010 on Chinese Windows (GBK encoding).
兼容 VC++2010，中文 Windows GBK 编码。

## Features / 功能

| # | Feature / 功能 | Description / 描述 |
|---|---------|-------------|
| 1 | Flight Info Entry / 航班信息录入 | Admin adds flights with number, origin, destination, departure time, seat rows / 管理员录入航班号、出发地、目的地、起飞时间、座位排数 |
| 2 | Flight Info Browse / 航班信息浏览 | Admin views all flights with full details / 管理员浏览所有航班的详细信息 |
| 3 | Flight Info Search / 航班信息搜索 | Search by flight number, origin city, or destination city / 按航班号、出发城市或目的城市搜索 |
| 4 | Book Ticket / 订票 | Passenger selects flight, views seat map, chooses seat, enters name & ID / 乘客选择航班、查看座位图、选座、输入姓名与身份证号 |
| 5 | Sell Ticket / 售票 | Counter ticket sale with fare display / 柜台售票并显示票价 |
| 6 | Refund Ticket / 退票 | Cancel booking by name or ID card number, release seat / 按姓名或身份证号退票，释放座位 |
| 7 | Statistics / 统计 | Occupancy rate per flight and overall summary / 每航班上座率及总体汇总统计 |
| 8 | Query / 查询 | View flight details, seat map, passenger list (password-protected) / 查看航班详情、座位图、乘客清单（需密码验证） |

## Seat Layout / 座位布局

```
Window/窗                         Window/窗
         A     B     C     D
      +-----+-----+-----+-----+
   1  | [ ] | [ ] | [ ] | [ ] |
      +-----+-----+-----+-----+
   2  | [ ] | [X] | [ ] | [ ] |
      +-----+-----+-----+-----+
  ...
```

Legend / 图例: `[ ]` = Free / 空闲  `[X]` = Booked / 已订  A/D = Window seat / 靠窗座位

- Seats numbered as `row + column`, e.g. `10D` = Row 10, Seat D
  座位编号格式为 `排号 + 列号`，例如 `10D` = 第 10 排 D 座
- A and D are window seats / A 和 D 为靠窗座位
- B and C are aisle seats / B 和 C 为过道座位

## Build & Run / 编译运行

### VC++2010 (Visual Studio 2010)

Open the `.vcxproj` file directly, build with `Ctrl+Shift+B`, run with `F5`.
直接打开 `.vcxproj` 文件，按 `Ctrl+Shift+B` 编译，按 `F5` 运行。

### GCC (MinGW / Linux)

```bash
gcc -o main.exe main.c
./main.exe
```

### Requirements / 环境要求

- Pure C (C89/ANSI C), no C++ features / 纯 C 语言 (C89/ANSI C)，无 C++ 特性
- GBK encoding for Chinese characters / 中文字符采用 GBK 编码
- `//` comments supported by VC++2010 C mode / 使用 `//` 注释（VC++2010 C 模式支持）

## Usage / 使用说明

### Admin Portal / 管理员入口

| Menu / 菜单 | Action / 操作 |
|------|--------|
| 1. Add Flight Info / 添加航班信息 | Input flight number, cities, time, seat rows / 输入航班号、城市、起飞时间、座位排数 |
| 2. Browse All Flights / 浏览全部航班 | View all flights with seat availability / 查看所有航班及座位余量 |
| 3. Search Flights / 搜索航班 | Search by flight number / origin / destination / 按航班号/出发地/目的地搜索 |

### Passenger Portal / 乘客入口

| Menu / 菜单 | Action / 操作 |
|------|--------|
| 1. Book Ticket / 订票 | Choose flight -> view seats -> pick seat -> enter info / 选择航班 → 查看座位 → 选座 → 录入信息 |
| 2. Buy Ticket / 购票 | Counter purchase with fare / 柜台购票并显示票价 |
| 3. Refund Ticket / 退票 | Cancel by name or ID card number / 按姓名或身份证号退票 |
| 4. Statistics / 统计 | Occupancy rates for all flights / 所有航班上座率统计 |
| 5. Query / 查询 | Flight details, seat map, passenger list / 航班详情、座位图、乘客清单 |

### Viewing Passenger List / 查看乘客清单

Requires admin password: `admin123`
需要管理员密码：`admin123`

## File Structure / 文件结构

```
飞机航班订票系统/
  main.c              Main source code / 主源代码
  README.md           This file / 本文件
  .gitignore          Git ignore rules / Git 忽略规则
  *.vcxproj           VC++2010 project files / VC++2010 项目文件
```

## Data Structures / 数据结构

```
FlightSystem / 航班系统
  +-- Flight[0..99] / 航班
        +-- flightNo, origin, destination, departureTime / 航班号、出发地、目的地、起飞时间
        +-- totalRows, totalSeats, bookedCount / 总排数、总座位数、已订数
        +-- Seat[50][4]          (row x column matrix / 排×列矩阵)
        +-- Passenger[0..499]    (name, ID, seat / 姓名、身份证号、座位)
```

## License / 许可证

Educational project. / 教学项目。