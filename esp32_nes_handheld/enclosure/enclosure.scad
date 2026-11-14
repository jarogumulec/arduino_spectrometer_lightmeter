// ESP32 NES handheld - krabicka (vanicka + celni deska + hmatniky)
// Tisk: Prusa MK3S, tryska 0,4, vrstva 0,2. Rozmery v mm.
// Souradnice: x doprava, y nahoru (pohled zepredu), z = 0 spodni plocha
// celni desky, vnitrek do zapornych z. Pocatek xy = levy dolni vnitrni roh.
// Zdroj rozmeru: docs/enclosure.md (M = mereno 2026-11-12, O = vykres).
//
// part: "assembly" | "exploded" | "tray_layout" | "plate" | "tray" |"tray_layout" | "light_assembly" | "plate" | "tray" | "tray_light" |"tray_light" | "tray_light_caps" | "caps"| "exploded" | "plate" | "tray" | "caps"
//       (plate/tray/caps jsou v tiskove orientaci)
part = "assembly";
show_boards = true;      // makety desek v nahledu

$fn = 48;

// ---- obecne --------------------------------------------------------------
WALL = 2.4;     // stena vanicky
FLOOR = 2.0;    // dno vanicky
T = 2.4;        // tloustka celni desky
W = 153;        // vnitrni sirka
H = 60;         // vnitrni vyska
D = 30.5;       // vnitrni hloubka (z = 0 .. -D); 21 drzak + 9,5 pod zadni stranou displeje
R = 5;          // vnejsi polomer rohu
CL = 0.3;       // vule kolem desek
M3_TAP = 2.7;   // otvor pro M3 primo do plastu
M3_FREE = 3.4;  // pruchozi otvor M3
M2_FREE = 2.4;
SKIN = 0.8;     // plast nad slepym otvorem v celni desce

// ---- displej MSP2834 (O + M) ----------------------------------------------
// naležato, lista vlevo, slot SD u horni hrany
disp = [38, 9];              // levy dolni roh desky
disp_size = [86, 50];
disp_glass_h = 4.5;          // sklo nad licem desky (M)
disp_post_h = disp_glass_h + 0.2;
disp_holes = [[7, 3], [83, 3], [7, 47], [83, 47]];   // od leveho dolniho rohu (M)
disp_va = [[18.81, 3.2], [76.86, 46.8]];             // viditelna plocha (O)
win_margin = 0.5;
sd_center_x = 44.5;          // stred slotu od hrany s listou (M: 36-53)
sd_notch = [18, 11];         // sirka, hloubka od roviny celni desky

// ---- joystick 5smerny (M) --------------------------------------------------
// lista nahore (smer UP)
joy = [9, 3];
joy_size = [25, 41];
joy_stick = [12.5, 20.5];    // stred packy na desce
joy_holes = [[3.5, 8.5], [21.5, 8.5], [3.5, 32.5], [21.5, 32.5]];  // odvozeno z rozteci 18 x 24, neovereno
joy_post_h = 4.5;            // packa (11 nad deskou) vycniva 11 - 4.5 - T = 4.1
joy_open = 11.5;             // ctvercovy otvor (mechanika cca 10 x 10)

// ---- tlacitka 1x4 mala (M + foto) -----------------------------------------
// svisle, K1 nahore: A, B, Start, Select
btn = [128, 11.15];
btn_size = [16, 37.7];
btn_holes = [[2.5, 2.35], [13.5, 2.35], [2.5, 35.35], [13.5, 35.35]];
btn_first = 7.5;             // stred S1 od horni hrany (M)
btn_pitch = 7.62;            // M: cca 7; z fotografie 7,6; predpoklad 0,3"
btn_sw_h = 6.0;              // vyska spinace nad deskou (M)
btn_post_h = 7.3;
cap_d = 6.0;                 // drik hmatniku
cap_hole = 6.5;
cap_flange_d = 7.2;
cap_flange_h = 1.6;
cap_socket = [4.0, 0.6];     // prumer, hloubka zahloubeni pro hmatnik spinace
cap_out = 2.0;               // presah nad celni desku

// ---- ESP32 DevKit (M) -------------------------------------------------------
esp = [48, 30];              // levy dolni roh, delsi strana vodorovne
esp_size = [52, 28];
esp_hole_pitch = [46.5, 23.5];
esp_post_h = 10.5;           // koliky cca 8,5 + 2 mm pro pajene vodice
esp_post_d = 5.5;

// ---- napajeni -----------------------------------------------------------------
tp = [10, 0];                // TP4056: x leveho okraje; horni hrana u steny
tp_size = [17.3, 27.5];      // sirka O (neovereno), delka M
usb_open = [10, 4.4];        // otvor USB-C: sirka, vyska
usb_z0 = 0.8;                // spodni hrana otvoru nad dnem
mt = [126, 4];               // MT3608, nastojato vpravo
mt_size = [17, 36];
pocket_h = 2.5;

sw_x = 118;                  // vypinac v horni stene: stred
sw_z = -16;
sw_slot = [10, 5];           // M: zdvih 9, packa 4 x 5
sw_hole_pitch = 19;          // M
sw_body = [16, 8, 11];       // jen maketa
sw_flange = 23;

bat = [47, 1];               // drzak 18650 uvnitr na dne, levy dolni roh; otevrenou stranou k displeji
bat_size = [76, 21, 21];
bat_pocket_h = 5;

spk_c = [26, 17];            // rezerva pro reproduktor
spk_d = 28;
spk_ring_h = 2.0;

boss_xy = [[3.5, 3.5], [W - 3.5, 3.5], [3.5, H - 3.5], [W - 3.5, H - 3.5]];  // presah do steny, ne jen dotyk
boss_d = 8;

// ---- pomocne moduly -------------------------------------------------------
module rrect(w, h, r) {
  hull() for (x = [r, w - r], y = [r, h - r]) translate([x, y]) circle(r);
}

module outer2d() { translate([-WALL, -WALL]) rrect(W + 2 * WALL, H + 2 * WALL, R); }
module inner2d() { rrect(W, H, max(R - WALL, 0.5)); }

// sloupek smerem dolu z celni desky se slepym otvorem pro M3
module plate_post(d, h) {
  difference() {
    translate([0, 0, -h]) cylinder(d = d, h = h + 0.01);
    translate([0, 0, -h - 0.01]) cylinder(d = M3_TAP, h = h + T - SKIN);
  }
}
module plate_post_hole(h) {
  translate([0, 0, -h - 0.01]) cylinder(d = M3_TAP, h = h + T - SKIN);
}

// rohove dorazy kapsy pro desku lx x ly (na dne vanicky)
module corner_pocket(lx, ly, h = pocket_h, leg = 5, t = 1.6) {
  for (cx = [0, 1], cy = [0, 1])
    translate([cx * lx, cy * ly]) mirror([cx, 0]) mirror([0, cy])
      translate([-CL - t, -CL - t]) linear_extrude(h) difference() {
        square([leg + t, leg + t]);
        translate([t, t]) square([leg + 1, leg + 1]);
      }
}

function btn_y(i) = btn[1] + btn_size[1] - btn_first - i * btn_pitch;
btn_x = btn[0] + btn_size[0] / 2;

// ---- celni deska ----------------------------------------------------------
module plate() {
  difference() {
    union() {
      linear_extrude(T) outer2d();
      for (p = disp_holes) translate([disp[0] + p[0], disp[1] + p[1]]) plate_post(7, disp_post_h);
      for (p = joy_holes) translate([joy[0] + p[0], joy[1] + p[1]]) plate_post(6.5, joy_post_h);
      for (p = btn_holes) translate([btn[0] + p[0], btn[1] + p[1]]) plate_post(5, btn_post_h);
    }
    // okno displeje
    translate([disp[0] + disp_va[0][0] - win_margin, disp[1] + disp_va[0][1] - win_margin, -1])
      cube([disp_va[1][0] - disp_va[0][0] + 2 * win_margin,
            disp_va[1][1] - disp_va[0][1] + 2 * win_margin, T + 2]);
    // joystick
    translate([joy[0] + joy_stick[0], joy[1] + joy_stick[1], T / 2])
      cube([joy_open, joy_open, T + 2], center = true);
    // hmatniky
    for (i = [0:3]) translate([btn_x, btn_y(i), -1]) cylinder(d = cap_hole, h = T + 2);
    // srouby do vanicky
    for (p = boss_xy) translate([p[0], p[1], -1]) cylinder(d = M3_FREE, h = T + 2);
    // slepe otvory sloupku (prodlouzeni do desky)
    for (p = disp_holes) translate([disp[0] + p[0], disp[1] + p[1]]) plate_post_hole(disp_post_h);
    for (p = joy_holes) translate([joy[0] + p[0], joy[1] + p[1]]) plate_post_hole(joy_post_h);
    for (p = btn_holes) translate([btn[0] + p[0], btn[1] + p[1]]) plate_post_hole(btn_post_h);
  }
}

// ---- vanicka --------------------------------------------------------------
module tray() {
  esp_c = [esp[0] + esp_size[0] / 2, esp[1] + esp_size[1] / 2];
  tp_y = H - CL - tp_size[1];
  difference() {
    union() {
      // plast
      difference() {
        translate([0, 0, -D - FLOOR]) linear_extrude(D + FLOOR) outer2d();
        translate([0, 0, -D]) linear_extrude(D + 1) inner2d();
      }
      // rohove sloupky pro celni desku
      for (p = boss_xy) translate([p[0], p[1], -D]) cylinder(d = boss_d, h = D);
      // ESP32
      for (sx = [-1, 1], sy = [-1, 1])
        translate([esp_c[0] + sx * esp_hole_pitch[0] / 2, esp_c[1] + sy * esp_hole_pitch[1] / 2, -D])
          cylinder(d = esp_post_d, h = esp_post_h);
      // kapsy TP4056 a MT3608
      translate([tp[0], tp_y, -D]) corner_pocket(tp_size[0], tp_size[1]);
      translate([mt[0], mt[1], -D]) corner_pocket(mt_size[0], mt_size[1]);
      // dorazy drzaku baterie
      translate([bat[0], bat[1], -D]) corner_pocket(bat_size[0], bat_size[1], h = bat_pocket_h, leg = 7);
      // prstenec reproduktoru
      translate([spk_c[0], spk_c[1], -D]) difference() {
        cylinder(d = spk_d + 2 * CL + 3.2, h = spk_ring_h);
        translate([0, 0, -1]) cylinder(d = spk_d + 2 * CL, h = spk_ring_h + 2);
      }
    }
    // otvory rohovych sloupku
    for (p = boss_xy) translate([p[0], p[1], -12]) cylinder(d = M3_TAP, h = 13);
    // otvory sloupku ESP32
    for (sx = [-1, 1], sy = [-1, 1])
      translate([esp_c[0] + sx * esp_hole_pitch[0] / 2, esp_c[1] + sy * esp_hole_pitch[1] / 2,
                 -D + esp_post_h - 8]) cylinder(d = M3_TAP, h = 9);
    // vyrez pro microSD (otevreny k celni desce)
    translate([disp[0] + sd_center_x - sd_notch[0] / 2, H - 1, -sd_notch[1]])
      cube([sd_notch[0], WALL + 2, sd_notch[1] + 1]);
    // USB-C nabijeni
    translate([tp[0] + tp_size[0] / 2 - usb_open[0] / 2, H - 1, -D + usb_z0])
      cube([usb_open[0], WALL + 2, usb_open[1]]);
    // vypinac: draha packy + 2 srouby M2
    translate([sw_x - sw_slot[0] / 2, H - 1, sw_z - sw_slot[1] / 2])
      cube([sw_slot[0], WALL + 2, sw_slot[1]]);
    for (s = [-1, 1]) translate([sw_x + s * sw_hole_pitch / 2, H - 1, sw_z])
      rotate([-90, 0, 0]) cylinder(d = M2_FREE, h = WALL + 2);
    // mrizka reproduktoru
    for (gx = [-9:3.6:9], gy = [-9:3.6:9])
      if (norm([gx, gy]) <= 10.5)
        translate([spk_c[0] + gx, spk_c[1] + gy, -D - FLOOR - 1]) cylinder(d = 2, h = FLOOR + 2, $fn = 16);
  }
}

// ---- vanicka, odlehcena zkusebni verze ---------------------------------------
// Stejny vnejsi obrys, rohove sloupky a vsechny dorazy jako tray(), takze pasuje
// na stejnou celni desku. Uspora: stena 1,6 mm, dno 1,0 mm s okny, bocni a spodni
// stena jen jako nizky lem (plna vyska zustava u rohu a v horni stene s otvory),
// misto mrizky reproduktoru jeden otvor.
L_WALL = 1.6;
L_FLOOR = 1.1;    // 0,2 + 3 x 0,3 mm
L_RIM = 6;        // vyska nizkeho lemu
L_CORNER = 14;    // delka plne steny od rohu
// okna ve dne: [x0, y0, x1, y1]
L_WINDOWS = [
  [tp[0] + 3, H - CL - tp_size[1] + 3, tp[0] + tp_size[0] - 3, H - 4],     // pod TP4056
  [bat[0] + 4, bat[1] + 4, bat[0] + bat_size[0] - 4, bat[1] + bat_size[1] - 4],
  [mt[0] + 3, mt[1] + 3, mt[0] + mt_size[0] - 3, mt[1] + mt_size[1] - 3],
  [esp[0] + 7, esp[1] - 3, esp[0] + esp_size[0] - 7, esp[1] + esp_size[1] - 1],
  [102, 26, 122, 56],
  [31, 36, 44, 56],
];

module tray_light_base() {
  esp_c = [esp[0] + esp_size[0] / 2, esp[1] + esp_size[1] / 2];
  tp_y = H - CL - tp_size[1];
  difference() {
    union() {
      // dno
      translate([0, 0, -D - L_FLOOR]) linear_extrude(L_FLOOR) outer2d();
      // steny
      difference() {
        translate([0, 0, -D]) linear_extrude(D) difference() {
          outer2d();
          offset(delta = WALL - L_WALL) inner2d();
        }
        // snizeni spodni a bocnich sten na lem
        translate([L_CORNER, -WALL - 1, -D + L_RIM]) cube([W - 2 * L_CORNER, WALL + 2, D]);
        for (x = [-WALL - 1, W - 1])
          translate([x, L_CORNER, -D + L_RIM]) cube([WALL + 2, H - 2 * L_CORNER, D]);
      }
      for (p = boss_xy) translate([p[0], p[1], -D]) cylinder(d = boss_d, h = D);
      for (sx = [-1, 1], sy = [-1, 1])
        translate([esp_c[0] + sx * esp_hole_pitch[0] / 2, esp_c[1] + sy * esp_hole_pitch[1] / 2, -D])
          cylinder(d = esp_post_d, h = esp_post_h);
      translate([tp[0], tp_y, -D]) corner_pocket(tp_size[0], tp_size[1]);
      translate([mt[0], mt[1], -D]) corner_pocket(mt_size[0], mt_size[1]);
      translate([bat[0], bat[1], -D]) corner_pocket(bat_size[0], bat_size[1], h = bat_pocket_h, leg = 7);
      translate([spk_c[0], spk_c[1], -D]) difference() {
        cylinder(d = spk_d + 2 * CL + 3.2, h = spk_ring_h);
        translate([0, 0, -1]) cylinder(d = spk_d + 2 * CL, h = spk_ring_h + 2);
      }
    }
    for (p = boss_xy) translate([p[0], p[1], -12]) cylinder(d = M3_TAP, h = 13);
    for (sx = [-1, 1], sy = [-1, 1])
      translate([esp_c[0] + sx * esp_hole_pitch[0] / 2, esp_c[1] + sy * esp_hole_pitch[1] / 2,
                 -D + esp_post_h - 8]) cylinder(d = M3_TAP, h = 9);
    translate([disp[0] + sd_center_x - sd_notch[0] / 2, H - 1, -sd_notch[1]])
      cube([sd_notch[0], WALL + 2, sd_notch[1] + 1]);
    translate([tp[0] + tp_size[0] / 2 - usb_open[0] / 2, H - 1, -D + usb_z0])
      cube([usb_open[0], WALL + 2, usb_open[1]]);
    translate([sw_x - sw_slot[0] / 2, H - 1, sw_z - sw_slot[1] / 2])
      cube([sw_slot[0], WALL + 2, sw_slot[1]]);
    for (s = [-1, 1]) translate([sw_x + s * sw_hole_pitch / 2, H - 1, sw_z])
      rotate([-90, 0, 0]) cylinder(d = M2_FREE, h = WALL + 2);
    // okna ve dne
    for (w = L_WINDOWS) translate([0, 0, -D - L_FLOOR - 1])
      linear_extrude(L_FLOOR + 2) l_window2d(w);
    translate([spk_c[0], spk_c[1], -D - L_FLOOR - 1]) cylinder(d = spk_d - 4, h = L_FLOOR + 2);
  }
}

// mrizka v oknech dna: zebra 1,2 mm v rastru 8 mm, vyska 0,5 mm (prvni dve vrstvy pri 0,3 mm)
L_GRID_W = 1.2;
L_GRID_PITCH = 8;
L_GRID_H = 0.5;

module l_window2d(w) {
  translate([w[0] + 1.5, w[1] + 1.5]) offset(r = 1.5) square([w[2] - w[0] - 3, w[3] - w[1] - 3]);
}

module tray_light() {
  tray_light_base();
  translate([0, 0, -D - L_FLOOR]) linear_extrude(L_GRID_H) intersection() {
    offset(delta = 0.3) union() {
      for (w = L_WINDOWS) l_window2d(w);
      translate(spk_c) circle(d = spk_d - 4);
    }
    union() {
      for (x = [4 : L_GRID_PITCH : W]) translate([x - L_GRID_W / 2, -5]) square([L_GRID_W, H + 10]);
      for (y = [3 : L_GRID_PITCH : H]) translate([-5, y - L_GRID_W / 2]) square([W + 10, L_GRID_W]);
    }
  }
}

// ---- hmatnik ----------------------------------------------------------------
// tiskova orientace: limec dole
module cap() {
  difference() {
    union() {
      cylinder(d = cap_flange_d, h = cap_flange_h);
      cylinder(d = cap_d, h = cap_flange_h + T + cap_out - 0.6);
      translate([0, 0, cap_flange_h + T + cap_out - 0.6]) cylinder(d1 = cap_d, d2 = cap_d - 1.2, h = 0.6);
    }
    translate([0, 0, -0.01]) cylinder(d = cap_socket[0], h = cap_socket[1]);
  }
}

// ---- makety desek (jen nahled) ------------------------------------------------
module boards(plate_side = true, tray_side = true) {
  if (plate_side) {
  // displej
  color("darkgreen") translate([disp[0], disp[1], -disp_post_h - 1.6]) cube([86, 50, 1.6]);
  color("black") translate([disp[0] + 86 - 6.4 - 69.2, disp[1], -disp_post_h]) cube([69.2, 50, disp_glass_h]);
  color("gold") translate([disp[0] + 0.7, disp[1] + 8.49, -disp_post_h - 1.6 - 8.4]) cube([2.6, 33, 8.4]);
  color("silver") translate([disp[0] + 36, disp[1] + 50 - 3 - 15, -disp_post_h - 1.6 - 2]) cube([17, 15, 2]);
  // joystick
  color("teal") translate([joy[0], joy[1], -joy_post_h - 1.6]) cube([25, 41, 1.6]);
  color("dimgray") translate([joy[0] + joy_stick[0], joy[1] + joy_stick[1], -joy_post_h]) {
    translate([-5, -5, 0]) cube([10, 10, 5]);
    cylinder(d = 8, h = 11);
  }
  color("gold") translate([joy[0] + 2, joy[1] + 41, -joy_post_h - 1.6 - 2.5]) cube([21, 9, 2.5]);
  // tlacitka
  color("green") translate([btn[0], btn[1], -btn_post_h - 1.6]) cube([16, 37.7, 1.6]);
  for (i = [0:3]) {
    color("dimgray") translate([btn_x - 3, btn_y(i) - 3, -btn_post_h]) cube([6, 6, 3.5]);
    color("red") translate([btn_x, btn_y(i), -btn_post_h + btn_sw_h - cap_socket[1]]) cap();
  }
  }
  if (tray_side) {
  // ESP32
  color("navy") translate([esp[0], esp[1], -D + esp_post_h]) cube([52, 28, 1.6]);
  color("silver") translate([esp[0] + 28, esp[1] + 5, -D + esp_post_h + 1.6]) cube([18, 18, 3.4]);
  color("gold") for (y = [1.3, 28 - 1.3 - 2.5])
    translate([esp[0] + 7, esp[1] + y, -D + esp_post_h - 8.5]) cube([38, 2.5, 8.5]);
  // TP4056, MT3608
  color("royalblue") translate([tp[0], H - CL - tp_size[1], -D]) cube([tp_size[0], tp_size[1], 1.2]);
  color("silver") translate([tp[0] + tp_size[0] / 2 - 4.5, H - CL - 7, -D + 1.2]) cube([9, 8, 3.2]);
  color("royalblue") translate([mt[0], mt[1], -D]) cube([mt_size[0], mt_size[1], 6.5]);
  // vypinac
  color("silver") translate([sw_x - sw_body[0] / 2, H - sw_body[2], sw_z - sw_body[1] / 2]) cube(sw_body);
  color("silver") translate([sw_x - sw_flange / 2, H - 0.8, sw_z - 4]) cube([sw_flange, 0.8, 8]);
  color("black") translate([sw_x - 4.5, H, sw_z - 2]) cube([5, 7, 4]);
  // drzak 18650
  color("#333") translate([bat[0], bat[1], -D]) cube(bat_size);
  // reproduktor
  color("gray") translate([spk_c[0], spk_c[1], -D]) cylinder(d = spk_d, h = 5);
  }
}

// ---- vystup -------------------------------------------------------------------
if (part == "assembly") {
  color("gainsboro") tray();
  color("orange", 0.55) plate();
  if (show_boards) boards();
} else if (part == "exploded") {
  color("gainsboro") tray();
  translate([0, 0, 45]) color("orange") plate();
  if (show_boards) boards();
} else if (part == "tray_layout") {
  color("gainsboro") tray();
  boards(plate_side = false);
} else if (part == "plate") {
  translate([0, 0, T]) rotate([180, 0, 0]) plate();     // licem dolu
} else if (part == "tray") {
  translate([0, 0, D + FLOOR]) tray();                  // dnem dolu
} else if (part == "tray_light") {
  translate([0, 0, D + L_FLOOR]) tray_light();          // dnem dolu
} else if (part == "tray_light_caps") {
  // tiskova sada: odlehcena vanicka + 4 hmatniky vedle ni
  translate([0, 0, D + L_FLOOR]) tray_light();
  for (i = [0:3]) translate([40 + i * 15, -WALL - 12, 0]) cap();
} else if (part == "light_assembly") {
  color("gainsboro") tray_light();
  color("orange", 0.55) plate();
  if (show_boards) boards();
} else if (part == "caps") {
  for (i = [0:3]) translate([i * 11, 0, 0]) cap();
}
