pub mod fov {
    use game::game::*;
    use std::f64;

    #[derive(Clone)]
    pub enum GlareType {
        Unseen,
        Seen,
        Lit,
    }

    /*Manhattan distance, because diag*/
    fn diag_dist(p1: (i32, i32), p2: (i32, i32)) -> f64 {
        let dx = p2.0 - p1.0;
        let dy = p2.1 - p1.0;
        (dx as f64).abs().max((dy as f64).abs())
    }

    /*Linear interpolation between two coordinate tuples*/
    fn tile_lerp(s: (i32, i32), e: (i32, i32), t: f64) -> (i32, i32) {
        let s = (s.0 as f64, s.1 as f64);
        let e = (e.0 as f64, e.1 as f64);
        let flerp = (lerp(s.0, e.0, t), lerp(s.1, e.1, t));
        (flerp.0.round() as i32, flerp.1.round() as i32)
    }

    fn lerp(s: f64, e: f64, t: f64) -> f64 {
        s + t * (e - s)
    }

    /*Angle to direction vector of magnitude length, added to the starting position
     * Note: The length is halved in the y direction, because tiles have double height as width
     * */
    fn ray(pos: (i32, i32), angle: f64, len: f64) -> (i32, i32) {
        let dir = (
            (f64::cos(angle) * len).round(),
            (f64::sin(angle) * len * 0.5).round(),
        );
        (dir.0 as i32 + pos.0, dir.1 as i32 + pos.1)
    }

    /*FOV Algorithm: Raycasts in 360 degrees until a wall is met
     * Raycasting algorithm:
     * because fov is over a grid of discrete tile sizes, you can find the distance
     * between the start and end of the ray and iterate over the tiles that the ray intersects,
     * stopping when a wall is met.*/
    pub fn fov(map: &mut Map, pos: (u32, u32), dist: u32) {
        for tile in &mut map.lighting {
            if let GlareType::Lit = tile {
                *tile = GlareType::Seen;
            }
        }

        let pos = (pos.0 as i32, pos.1 as i32);
        let dist = dist as f64;
        for angle in 0..360 {
            let rad: f64 = (angle as f64) * (f64::consts::PI / 180.0);
            let (nx, ny) = ray(pos, rad, dist);
            let n = diag_dist(pos, (nx, ny)).round() as usize;

            for j in 0..=n {
                let t: f64 = if n == 0 { 0.0 } else { j as f64 / n as f64 };
                let (tx, ty) = tile_lerp(pos, (nx, ny), t);
                if tx < 0 || ty < 0 || tx >= (map.width as i32) || ty >= (map.height as i32) {
                    continue;
                }
                let idx = tx as usize + (ty as usize * map.width);
                if map.get_tile(tx as usize, ty as usize) != Tile::Floor {
                    map.lighting[idx] = GlareType::Seen;
                    break;
                }
                if tx == pos.0 && ty == pos.1 {
                    continue;
                }

                map.lighting[idx] = GlareType::Lit;
            }
        }
    }

}
