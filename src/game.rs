extern crate ncurses;

pub mod game {
    use ncurses::*;
    use fov::fov::*;
    use path::path::*;

    pub enum ObjectType {
        Door,
        Player,
        Enemy,
        End,
    }

    pub struct Object {
        pub y: u32, pub x: u32,
        pub dy: i32, pub dx: i32,
        pub ch: u8,
        pub otype: ObjectType,
        pub update: Option<fn(&mut Object, i32)>,
    }

    #[derive(Copy, Clone)]
    pub struct ObjectHandle {
        pub handle: u32,
    }

    impl ObjectHandle {
        pub fn new(cnt: u16, idx: u16) -> Self {
            ObjectHandle {
                handle: ((cnt as u32) << 16) | (idx as u32),
            }
        }
        pub fn indice(&self) -> usize {
            (self.handle & 0xffff) as usize
        }
    }

    #[derive(Debug, Copy, Clone, PartialEq)]
    pub enum Tile {
        Floor,
        Wall,
    }

    pub struct Map {
        pub objects: Vec<(Box<Object>, ObjectHandle)>,
        pub width: usize, pub height: usize,
        //pub map: Vec<Tile>,
        pub map: Vec<(Tile, u8)>,
        pub nodes: Vec<Box<Node>>,
        pub lighting: Vec<GlareType>,
    }

    impl Map {
        pub fn new(w: usize, h: usize) -> Self {
            Map {
                objects: Vec::new(),
                width:w, height: h,
                map: vec![(Tile::Floor, b'.');w*h],
                lighting: vec![GlareType::Unseen; w*h],
                nodes: Vec::new(),
            }
        }

        pub fn get_neighbors(dimensions: &(usize, usize), n: usize) -> Vec<usize> {
            let width = dimensions.0;
            let height = dimensions.1;
            let mut neighbors: Vec<usize> = Vec::new();
            let x = n % width;
            let y = n / width;

            if x > 1 {
                neighbors.push(x-1 + y * width);
            }
            if x < width-1 {
                neighbors.push(x+1 + y * width);
            }
            if y > 1 {
                neighbors.push(x + (y-1) * width);
            }
            if y < height-1 {
                neighbors.push(x + (y+1) * width);
            }

            return neighbors;
        }
        /*Create a pathfinding map from the current state*/
        pub fn path_map(&mut self) {
            self.nodes.clear();
            for i in 0..self.height {
                for j in 0..self.width {
                    let wall = self.get_tile(j, i) == Tile::Wall;
                    self.nodes.push(Box::new(Node::new(j as u32, i as u32, wall)));
                }
            }
        }

        /*Returns the value of 2d indexed tile in the map*/
        pub fn get_tile(&self, x: usize, y: usize) -> Tile {
            self.map[x + y * self.width].0
        }

        /*Returns mutable reference to 2d indexed tile in the map*/
        pub fn get_tile_mut(&mut self, x: usize, y: usize) -> &mut (Tile, u8) {
            &mut self.map[x + y * self.width]
        }

        /*Prints the ascii map with ncurses*/
        pub fn render(&self) {
            let w = self.width;
            /*Print the map*/
            attron(COLOR_PAIR(1));
            for (cnt, tile) in self.map.iter().enumerate() {
                if let GlareType::Unseen = self.lighting[cnt] {
                    attron(COLOR_PAIR(4));
                } else if let GlareType::Lit = self.lighting[cnt] {
                    attron(COLOR_PAIR(2));
                } else {
                    attron(COLOR_PAIR(1));
                }
                mvaddch((cnt / w) as i32, (cnt % w) as i32, tile.1 as u32);
            }
            /*Print the game objects*/
            attron(COLOR_PAIR(2));
            for (obj, _) in &self.objects {
                mvaddch(obj.y as i32, obj.x as i32, obj.ch as u32);
            }
        }

        /*Updates the game objects*/
        pub fn update(&mut self, input: i32) {
            for i in 0..self.objects.len() {
                let handle = self.objects[i].1;
                if let Some(update) = self.objects[i].0.update {
                    (update)(&mut self.objects[i].0, input);
                }
                self.move_obj(handle);
            }
        }

        /*Get the position of an object*/
        pub fn get_obj_pos(&self, obj_handle: ObjectHandle) -> (u32, u32) {
            let (obj, _) = &self.objects[obj_handle.indice()];
            (obj.x, obj.y)
        }

        /*Check's if move is valid, and moves game object given the array indice*/
        pub fn move_obj(&mut self, obj_handle: ObjectHandle) -> bool {
            let (obj, _) = &mut self.objects[obj_handle.indice()];
            let nx = (obj.x as i32 + obj.dx) as u32;
            let ny = (obj.y as i32 + obj.dy) as u32;
            obj.dx = 0;
            obj.dy = 0;
            let index: usize = ((self.width as u32) * ny + nx) as usize;
            /*If tile is floor, then move is valid*/
            match self.map[index].0 {
                Tile::Floor => {
                    obj.x = nx;
                    obj.y = ny;
                    true
                }
                _ => false,
            }
        }

        /*Creates a game object and adds it to the object vector*/
        pub fn add_obj(&mut self, x: u32, y: u32, ch: u8, update: Option<fn(&mut Object, i32)>) -> ObjectHandle {
            let obj = Box::new(Object { x: x, y: y, dx: 0, dy: 0, ch: ch, update: update, otype: ObjectType::Player});
            let obj_handle = ObjectHandle::new(0, self.objects.len() as u16);
            self.objects.push((obj, obj_handle));
            return obj_handle;
        }
    }
}
