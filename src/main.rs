extern crate ncurses;

use ncurses::*;
use std::{thread, time};

mod fov;
mod game;
mod generate;
mod path;

use game::game::{Map, Object, ObjectType, ObjectHandle};
use generate::generate::generate_map;
use path::path::*;

fn init() {
    initscr();
    noecho();
    curs_set(CURSOR_VISIBILITY::CURSOR_INVISIBLE);
    cbreak();
    keypad(stdscr(), true);
    timeout(0); //Non-blocking getch()
    start_color();
    init_color(COLOR_BLACK, 0, 0, 0);
    init_color(COLOR_WHITE, 500, 500, 500);
    init_color(COLOR_YELLOW, 960, 880, 520);
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_BLACK, COLOR_YELLOW);
    init_pair(4, COLOR_BLACK, COLOR_BLACK);
    init_pair(4, COLOR_BLACK, COLOR_BLACK);

    attron(COLOR_PAIR(1));
}

fn destroy() {
    endwin();
}

fn player_update(player: &mut Object, input: i32) {
        let mut mx: i32 = 0;
        let mut my: i32 = 0;
        match input {
            KEY_UP => my = -1,
            KEY_DOWN => my = 1,
            KEY_RIGHT => mx = 1,
            KEY_LEFT => mx = -1,
            _ => (),
        }
        player.dx = mx;
        player.dy = my;
}

fn main() {
    init();

    let mut w = 80;
    let mut h = 42;
    getmaxyx(stdscr(), &mut h, &mut w);

    let mut map = Map::new(w as usize, h as usize);
    generate_map(&mut map);
    map.path_map();
    let start = ((w / 2) as u32, (h / 2) as u32);
    let player_h = map.add_obj(start.0, start.1, b'@', Some(player_update));

    loop {
        refresh();
        let input = getch();
        if input != -1 {
            if input == 'q' as i32 || input == 'Q' as i32 {
                break;
            }
            map.update(input);

            let pos = map.get_obj_pos(player_h);
            fov::fov::fov(&mut map, pos, 7);
        }

        map.render();
        attron(COLOR_PAIR(3));

        let pos = map.get_obj_pos(player_h);
        let eidx = start.0 as usize + (start.1 as usize) * map.width;
        let sidx = pos.0 as usize + (pos.1 as usize) * map.width;
        let dim = (map.width, map.height);
        find(sidx, eidx, &dim, &mut map.nodes, &Map::get_neighbors);
        let mut n = &map.nodes[eidx];
        while let Some(ni) = n.parent {
            mvaddch(n.y as i32, n.x as i32, '.' as u32);
            if ni == sidx || eidx == sidx {
                break;
            }
            n = &map.nodes[ni];
        }
        /*Otherwise this thread uses 100% of cpu core*/
        thread::sleep(time::Duration::from_millis(10));
    }
    destroy();
}
