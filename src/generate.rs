extern crate rand;

pub mod generate {
    extern crate rand;
    use super::rand::Rng;
    use game::game::{Map, Tile};

    enum WallType {
        WallNone,
        WallUp,
        WallDown,
        WallLeft,
        WallRight,
    }
    /*Wall orientation array
     * l = 1, r = 2, t = 4, b = 8
     * lrtb = 1111 = 16
    let idx2: usize = (bleft | bright << 1 | tleft << 2 | tright << 3) as usize;
     * */
    static WALLS: &'static [u8] =
        &[b'#', b'|',  b'|',  b'|',
          b'~', b'/',  b'\\', b'^',
          b'_', b'\\', b'/',  b'U',
          b'-', b'<',  b'>',  b'O',

          b'#', b'`',  b'\'', b'#',
          b',', b'#',  b'#',  b'#',
          b'.', b'#',  b'#',  b'#',
          b'#', b'#',  b'#',  b'#',
        ];

    fn random_max(max: u32) -> u32 {
        rand::thread_rng().gen_range(1, max + 1)
    }

    fn prob(odds: f32) -> bool {
        let n = rand::thread_rng().gen_range(1, 101);
        n as f32 > (odds * 100.0)
    }

    fn fit_rect(map: &Map, mut x: u32, mut y: u32, mut w: u32, mut h: u32) -> bool {
        /*If coorridor then extend long dimension to connect*/
        if w == 1 {
            h = h - 1;
            y = y + 1;
        } else if h == 1 {
            w = w - 1;
            x = x + 1;
        }
        /*Assert min/max falls in range of map*/
        if (x + w) >= map.width as u32 || x <= 1 {
            return false;
        }
        if (y + h) >= map.height as u32 || y <= 1 {
            return false;
        }

        /*Assert the rectangle space is unused*/
        for i in y - 1..=y + h {
            for j in x - 1..=x + w {
                if map.get_tile(j as usize, i as usize) == Tile::Floor {
                    return false;
                }
            }
        }
        return true;
    }

    fn fill_rect(map: &mut Map, x: u32, y: u32, w: u32, h: u32) {
        for i in y..y + h {
            for j in x..x + w {
                *map.get_tile_mut(j as usize, i as usize) = (Tile::Floor, b'.');
            }
        }
    }

    fn get_wall(map: &Map, x: &mut u32, y: &mut u32) -> WallType {
        let mut num_walls = 0;
        /*Count the number of walls (solid tiles with adjacent floor tiles)*/
        for i in 1..map.height - 1 {
            for j in 1..map.width - 1 {
                if map.get_tile(j, i) == Tile::Wall {
                    num_walls += (map.get_tile(j - 1, i) == Tile::Floor) as u32;
                    num_walls += (map.get_tile(j + 1, i) == Tile::Floor) as u32;
                    num_walls += (map.get_tile(j, i + 1) == Tile::Floor) as u32;
                    num_walls += (map.get_tile(j, i - 1) == Tile::Floor) as u32;
                }
            }
        }

        /*Pick random wall and set it's position return it's type */
        let mut wtype: WallType = WallType::WallNone;
        let mut idx = random_max(num_walls);
        for i in 1..map.height - 1 {
            for j in 1..map.width - 1 {
                if map.get_tile(j, i) == Tile::Wall {
                    if map.get_tile(j - 1, i) == Tile::Floor {
                        idx -= 1;
                        wtype = WallType::WallRight;
                    } else if map.get_tile(j + 1, i) == Tile::Floor {
                        idx -= 1;
                        wtype = WallType::WallLeft;
                    } else if map.get_tile(j, i - 1) == Tile::Floor {
                        idx -= 1;
                        wtype = WallType::WallDown;
                    } else if map.get_tile(j, i + 1) == Tile::Floor {
                        idx -= 1;
                        wtype = WallType::WallUp;
                    }
                    if idx == 0 {
                        *x = j as u32;
                        *y = i as u32;
                        return wtype;
                    }
                }
            }
        }
        return wtype;
    }

    /*Fill deadends, returns true if it found any*/
    fn fill_deadends(map: &mut Map) -> bool {
        let mut deadends = false;

        /*For every floor tile that has > 2 adjacent walls, it is a deadend*/
        for i in 0..map.height {
            for j in 0..map.width {
                if map.get_tile(j, i) != Tile::Floor {
                    continue;
                }
                let mut num_walls = 0;
                num_walls += (map.get_tile(j - 1, i) == Tile::Wall) as u32;
                num_walls += (map.get_tile(j + 1, i) == Tile::Wall) as u32;
                num_walls += (map.get_tile(j, i - 1) == Tile::Wall) as u32;
                num_walls += (map.get_tile(j, i + 1) == Tile::Wall) as u32;

                if num_walls > 2 {
                    *map.get_tile_mut(j, i) = (Tile::Wall, b'#');
                    deadends = true;
                }
            }
        }
        return deadends;
    }

    /*Generate specific wall characters for each orientation*/
    pub fn prettify(map: &mut Map) {

        for i in 1..map.height-1 {
            for j in 1..map.width-1 {
                if map.get_tile(j, i) != Tile::Wall {
                    continue;
                }
                let top = (map.get_tile(j, i-1) == Tile::Floor) as u32;
                let bot = (map.get_tile(j, i+1) == Tile::Floor) as u32;
                let left = (map.get_tile(j-1, i) == Tile::Floor) as u32;
                let right = (map.get_tile(j+1, i) == Tile::Floor) as u32;
                let tleft = (map.get_tile(j-1, i-1) == Tile::Floor) as u32;
                let tright = (map.get_tile(j+1, i-1) == Tile::Floor) as u32;
                let bleft = (map.get_tile(j-1, i+1) == Tile::Floor) as u32;
                let bright = (map.get_tile(j+1, i+1) == Tile::Floor) as u32;

                let idx: usize = (left | right << 1 | top << 2 | bot << 3) as usize;
                let idx2: usize = (bleft | bright << 1 | tleft << 2 | tright << 3) as usize;
                if idx == 15 {
                    map.get_tile_mut(j, i).1 = WALLS[idx2 + 16];
                } else {
                    map.get_tile_mut(j, i).1 = WALLS[idx];
                }
            }
        }
    }

    /*Procedurally Generate A Map*/
    pub fn generate_map(map: &mut Map) {
        /*Initialize Map to Wall Tile*/
        for t in &mut map.map {
            *t = (Tile::Wall, b'#');
        }

        /*Put starting room of random size in the middle*/
        let size: u32 = random_max(7) + 5;
        let startx = (map.width as u32) / 2 - size / 2;
        let starty = (map.height as u32) / 2 - size / 4;
        fill_rect(map, startx, starty, size, size / 2);

        /*Randomly generate rooms, and place them as long as they don't overlap*/
        let mut tries = 500;
        let mut rooms = 100;
        while rooms > 0 && tries > 0 {
            let mut wx: u32 = 0;
            let mut wy: u32 = 0;
            let wall = get_wall(map, &mut wx, &mut wy);
            if let WallType::WallNone = wall {
                continue;
            }

            /*50% chance it is a room, otherwise its a cooridoor*/
            let room = prob(0.6);
            let mut attempts = 1;

            /*Increasing attempts will make more compact maps, decreasing will space it out more*/
            while attempts > 0 {
                let (w, h): (u32, u32) = if room {
                    (random_max(3) + 3, random_max(3) + 3)
                } else {
                    match wall {
                        WallType::WallLeft | WallType::WallRight=> (1, random_max(6) + 1),
                        WallType::WallDown | WallType::WallUp => (random_max(6) + 1, 1),
                        WallType::WallNone => break,
                    }
                };
                if w > wx || h > wy {
                    break;
                }

                /*Each room extends in the opposite direction of the wall*/
                let (rx, ry, rw, rh) = match wall {
                    WallType::WallDown => (wx, wy + 1, w, h),
                    WallType::WallUp => (wx - w / 2, wy - h, w, h),
                    WallType::WallLeft => (wx - w, wy - h / 2, w, h),
                    WallType::WallRight => (wx + 1, wy - h / 2, w, h),
                    _ => (0, 0, 0, 0),
                };
                /*If the room fits, then fill it in*/
                if fit_rect(map, rx, ry, rw, rh) {
                    fill_rect(map, rx, ry, rw, rh);
                    rooms -= room as u32;
                    *map.get_tile_mut(wx as usize, wy as usize) = (Tile::Floor, b'.');
                    break;
                }
                attempts = attempts - 1;
            }
            tries = tries - 1;
        }
        while fill_deadends(map) {}
        prettify(map);
    }
}
