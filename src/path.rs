/*Pathfinding module with A*
 * Sorted vector instead of using a priority queue type*/
pub mod path {
    pub struct Node {
        pub x: u32,
        pub y: u32,
        pub wall: bool,
        g: u32,
        f: u32,
        h: u32,
        pub parent: Option<usize>,
    }
    impl Node {
        pub fn new(x: u32, y: u32, wall: bool) -> Self {
            Node {
                x: x,
                y: y,
                wall: wall,
                g: 0,
                f: 0,
                h: 0,
                parent: None,
            }
        }
        fn heuristic(&mut self, pos: (u32, u32)) {
            let dx = self.x as i32 - pos.0 as i32;
            let dy = self.y as i32 - pos.1 as i32;
            self.h = ((dx.pow(2) + dy.pow(2)) as f64).sqrt() as u32;
        }
    }
    /*A* Algorithm:
     * Loop through nodes neighbors and add them to a priority queue sorted by a distance
     * heuristic. Keep doing this for neighbors, but set parent of each node to the previous
     * node. When the loop happens upon the end node, it can traverse the parent pointers back
     * to the start and recreate the path. Guaranteed to be the shortest path */
    pub fn find<T>(
        start: usize,
        end: usize,
        map: &T,
        nvec: &mut [Box<Node>],
        get_neighbors: &Fn(&T, usize) -> Vec<usize>,
    ) {
        let mut open: Vec<usize> = Vec::new();
        let mut closed: Vec<usize> = Vec::new();

        let end_pos = (nvec[end].x, nvec[end].y);
        open.push(start);
        while let Some(cur) = open.pop() {
            if cur == end {
                break;
            }
            let cost = nvec[cur].g + 1;
            closed.push(cur);
            let mut neighbors = get_neighbors(map, cur);
            while let Some(n) = neighbors.pop() {
                if nvec[n].wall {
                    continue;
                }
                if let Some(_) = closed.iter().find(|a| **a == n) {
                    continue;
                }

                let (update, add) = match open.iter().find(|a| **a == n) {
                    Some(_) => (cost < nvec[n].g, false),
                    None => (true, true),
                };
                if add {
                    nvec[n].heuristic(end_pos);
                    open.push(n);
                }
                if update {
                    nvec[n].g = cost;
                    nvec[n].f = nvec[n].g + nvec[n].h;
                    nvec[n].parent = Some(cur);
                }
            }
            open.sort_unstable_by(|a, b| nvec[*b].f.cmp(&nvec[*a].f));
        }
    }
}
