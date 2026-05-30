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
