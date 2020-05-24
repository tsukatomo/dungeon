#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>

using std::cout;
using std::cerr;
using std::endl;

const int MIN_ROOM_SIZE = 5;
const int MIN_RECT_SIZE = MIN_ROOM_SIZE + 2;

class Dungeon {
    // Some structure
    public: 
        using MapType = char;
        enum : MapType{
            WALL = '0',
            BOUNDARY = '1',
            ROOM = 'r',
            AISLE = 'a'
        };
        struct Rect;
    // Constructor
    public:
        Dungeon();
        Dungeon(int width, int height);
        ~Dungeon();
    // Reference
    public:
        const int width() const;
        const int height() const;
        const Rect &rect_at(int i) const;
        Rect &rect_at(int i);
        const MapType &map_at(int x, int y) const;
        MapType &map_at(int x, int y);
    // Method
    public:
        // all processes
        void make_dungeon(int w, int h, int n_room, int n_branch);
        // rect-splitting
        void split_random(int n_room, int n_branch);
        int split_rect_vertically();
        int split_rect_horizontally();
        void shuffle_rectlist();
        void sort_rectlist();
        // room-making
        void make_room(int n_branch);
        // aisle-routing
        void make_aisle();
        int make_aisle_vertically(Rect *r1, Rect *r2);
        int make_aisle_horizontally(Rect *r1, Rect *r2);
        // other
        void reset_map(int w, int h);
        int int_rand(int min, int max);
    // Member
    private:
        int _width;
        int _height;
        std::vector<Rect> _rect;
        std::vector<std::vector<MapType> > _map;
};

// Map Output Stream
std::ostream &operator << (std::ostream &os, Dungeon &dun);

//--------------
//  Structure
//--------------
struct Dungeon::Rect{
    int x0;
    int y0;
    int x1;
    int y1;
    int room_x0;
    int room_y0;
    int room_x1;
    int room_y1;
};

//--------------
// Constructor
//--------------
Dungeon::Dungeon() :
    _width(0),
    _height(0),
    _rect(0),
    _map(0, std::vector<MapType>(0))
{
}

Dungeon::Dungeon(int width, int height) :
    _width(width),
    _height(height),
    _rect(1),
    _map(width, std::vector<MapType>(height, WALL))
{
    Rect *r_init;
    r_init = &rect_at(0);
    r_init -> x0 = 0;
    r_init -> y0 = 0;
    r_init -> x1 = width - 1;
    r_init -> y1 = height - 1;
}

Dungeon::~Dungeon(){
    _rect.clear();
    _map.clear();
}

//--------------
// Refernce
//--------------
const int Dungeon::width() const { return _width; }
const int Dungeon::height() const { return _height; }
const Dungeon::Rect &Dungeon::rect_at(int i) const { return _rect[i]; }
Dungeon::Rect &Dungeon::rect_at(int i) { return _rect[i]; }
const Dungeon::MapType &Dungeon::map_at(int x, int y) const { return _map[x][y]; }
Dungeon::MapType &Dungeon::map_at(int x, int y) { return _map[x][y]; }

//--------------
// Method
//--------------
void Dungeon::make_dungeon(int w, int h, int n_room, int n_branch){
    reset_map(w, h);
    split_random(n_room, n_branch);
    make_room(n_branch);
    make_aisle();
}

void Dungeon::split_random(int n_room, int n_branch){
    for (int i = 0; i < n_room + n_branch - 1; i++){
        sort_rectlist();
        switch (int_rand(0, 1)){
        case 0: // r->(r1/r2) > r->(r1|r2)
            if (split_rect_horizontally() == 1) split_rect_vertically();
            break;
        case 1: // r->(r1|r2) > r->(r1/r2)
            if (split_rect_vertically() == 1) split_rect_horizontally();
            break;
        default:
            break;
        }
    }
}

int Dungeon::split_rect_vertically(){
    // Pop back from rectangle vector -> r0
    Rect &r0 = _rect.back();  
    if (r0.x1 - r0.x0 + 1 < MIN_RECT_SIZE * 2 + 1) return 1;
    _rect.pop_back();
    // Choose boundary position
    int bx = int_rand(r0.x0 + MIN_RECT_SIZE, r0.x1 - MIN_RECT_SIZE);
    //int bx = (r0.x0 + r0.x1) / 2; // center
    // Split r0 -> r1 | r2
    Rect r1, r2;
    r1.x0 = r0.x0;
    r1.y0 = r0.y0;
    r1.x1 = bx - 1;
    r1.y1 = r0.y1;
    r2.x0 = bx + 1;
    r2.y0 = r0.y0;
    r2.x1 = r0.x1;
    r2.y1 = r0.y1;
    _rect.push_back(r1);
    _rect.push_back(r2);
    // Write boundary on dungeon
    for (int y = r0.y0; y <= r0.y1; y++){
        _map[bx][y] = BOUNDARY;
    }
    return 0;
}

int Dungeon::split_rect_horizontally(){
    // Pop back from rectangle vector -> r0
    Rect &r0 = _rect.back();
    if (r0.y1 - r0.y0 + 1 < MIN_RECT_SIZE * 2 + 1) return 1;
    _rect.pop_back();
    // Choose boundary position
    int by = int_rand(r0.y0 + MIN_RECT_SIZE, r0.y1 - MIN_RECT_SIZE);
    //int by = (r0.y0 + r0.y1) / 2; // center
    // Split r0 -> r1 / r2
    Rect r1, r2;
    r1.x0 = r0.x0;
    r1.y0 = r0.y0;
    r1.x1 = r0.x1;
    r1.y1 = by - 1;
    r2.x0 = r0.x0;
    r2.y0 = by + 1;
    r2.x1 = r0.x1;
    r2.y1 = r0.y1;
    _rect.push_back(r1);
    _rect.push_back(r2);
    // Write boundary on dungeon
    for (int x = r0.x0; x <= r0.x1; x++){
        _map[x][by] = BOUNDARY;
    }
    return 0;
}

void Dungeon::shuffle_rectlist(){
    std::random_device seed;
	std::mt19937 engine(seed());
	std::shuffle(_rect.begin(), _rect.end(), engine);
}

void Dungeon::sort_rectlist(){
    std::sort(_rect.begin(), _rect.end(), [](Rect r1, Rect r2){
        return std::max(r1.x1-r1.x0+1, r1.y1-r1.y0+1) < std::max(r2.x1-r2.x0+1, r2.y1-r2.y0+1);
    });
}

void Dungeon::make_room(int n_branch){
    // shuffle rectangle list
    shuffle_rectlist();
    // Make a room or a branch for each rectangles
    for (int i = 0; i < _rect.size(); i++){
        Rect *r = &_rect[i];
        /*
        r->room_x0 = r->x0 + 1;
        r->room_y0 = r->y0 + 1;
        r->room_x1 = r->x1 - 1;
        r->room_y1 = r->y1 - 1;
        */
        // make branch
        if (i < n_branch){
            r->room_x0 = int_rand(r->x0+1, r->x1-1);
            r->room_y0 = int_rand(r->y0+1, r->y1-1);
            r->room_x1 = r->room_x0;
            r->room_y1 = r->room_y0;
            _map[r->room_x0][r->room_y0] = AISLE;
        }
        // make room
        else{
            r->room_x0 = int_rand(r->x0+1, (r->x0 + r->x1)/2-MIN_ROOM_SIZE/2);
            r->room_y0 = int_rand(r->y0+1, (r->y0 + r->y1)/2-MIN_ROOM_SIZE/2);
            r->room_x1 = int_rand((r->x0 + r->x1)/2+MIN_ROOM_SIZE/2, r->x1-1);
            r->room_y1 = int_rand((r->y0 + r->y1)/2+MIN_ROOM_SIZE/2, r->y1-1);
            for (int y = r->room_y0; y <= r->room_y1; y++){
                for (int x = r->room_x0; x <= r->room_x1; x++){
                    _map[x][y] = ROOM;    
                }
            }
        }
    }
}

void Dungeon::make_aisle(){
    Rect *r1, *r2;
    shuffle_rectlist();
    for (int i = 0; i < _rect.size(); i++){
        for (int j = 0; j < _rect.size(); j++){
            if (i == j) continue;
            r1 = &_rect[i];
            r2 = &_rect[j];
            // if r1 / r2, make vertical aisle
            if (r2->x0 < r1->x1 && r1->x0 < r2->x1 && r2->y0 == r1->y1 + 2){
                make_aisle_vertically(r1, r2);
            }
            // if r1 | r2, make horizontal aisle
            else if (r2->x0 == r1->x1 + 2 && r2->y0 < r1->y1 && r1->y0 < r2->y1){
                make_aisle_horizontally(r1, r2);
            }
        }
    }
}

int Dungeon::make_aisle_vertically(Rect *r1, Rect *r2){
    /*  r1  */
    /* ---- */
    /*  r2  */
    // Check if two rectangles are adjacent vertically
    if (r1->x1 <= r2->x0 || r2->x1 <= r1->x0) return 1;
    if (r2->y0 != r1->y1 + 2) return 1;
    // Choose start & end point of aisle
    int aisle_start_x = int_rand(r1->room_x0, r1->room_x1);
    int aisle_start_y = r1->room_y1 + 1;
    int aisle_end_x = int_rand(r2->room_x0, r2->room_x1);
    int aisle_end_y = r2->room_y0 - 1;
    // Check if aisle is already exist or not
    int check_start = aisle_start_x < aisle_end_x ? aisle_start_x : aisle_end_x;
    int check_end = aisle_start_x >= aisle_end_x ? aisle_start_x : aisle_end_x;
    for (int x = check_start - 1; x <= check_end + 1; x++){
        if (_map[x][r1->y1+1] != BOUNDARY){
            // cout << "Cannot make Vertical aisle: Aisle is already exist." << endl;
            return 1;
        }
    }
    // Make aisle
    for (int x = check_start; x <= check_end; x++){
        _map[x][r1->y1+1] = AISLE;
    }
    for (int y = aisle_start_y; y <= r1->y1; y++){
        _map[aisle_start_x][y] = AISLE;
    }
    for (int y = r2->y0; y <= aisle_end_y; y++){
        _map[aisle_end_x][y] = AISLE;
    }
    return 0;
}

int Dungeon::make_aisle_horizontally(Rect *r1, Rect *r2){
    /*    |    */
    /* r1 | r2 */
    /*    |    */
    // Check if two rectangles are adjacent vertically
    if (r1->y1 <= r2->y0 || r2->y1 <= r1->y0) return 1;
    if (r2->x0 != r1->x1 + 2) return 1;
    // Choose start & end point of aisle
    int aisle_start_x = r1->room_x1 + 1;
    int aisle_start_y = int_rand(r1->room_y0, r1->room_y1);
    int aisle_end_x = r2->room_x0 - 1;
    int aisle_end_y = int_rand(r2->room_y0, r2->room_y1);
    // Check if aisle is already exist or not
    int check_start = aisle_start_y < aisle_end_y ? aisle_start_y : aisle_end_y;
    int check_end = aisle_start_y >= aisle_end_y ? aisle_start_y : aisle_end_y;
    for (int y = check_start - 1; y <= check_end + 1; y++){
        if (_map[r1->x1+1][y] != '1'){
            // cout << "Cannot make Horizontal aisle: Aisle is already exist." << endl;
            return 1;
        }
    }
    // Make aisle
    for (int y = check_start; y <= check_end; y++){
        _map[r1->x1+1][y] = AISLE;
    }
    for (int x = aisle_start_x; x <= r1->x1; x++){
        _map[x][aisle_start_y] = AISLE;
    }
    for (int x = r2->x0; x <= aisle_end_x; x++){
        _map[x][aisle_end_y] = AISLE;
    }
    return 0;
}

void Dungeon::reset_map(int w, int h){
    // resize
    _map.resize(w);
    for (auto &col: _map){
        col.resize(h);
    }
    _width = w;
    _height = h;
    // reset map
    for (int y = 0; y < _height; y++){
        for (int x = 0; x < _width; x++){
            _map[x][y] = WALL;
        }
    }
    // reset rectangle
    _rect.resize(1);
    Rect *r_init = &_rect[0];
    r_init -> x0 = 0;
    r_init -> y0 = 0;
    r_init -> x1 = _width - 1;
    r_init -> y1 = _height - 1;
}

int Dungeon::int_rand(int min, int max){
    std::random_device seed;
	std::mt19937 engine(seed());
    std::uniform_int_distribution<> rand(min, max);
    return rand(engine);
}

//-----------------------------------
// Stream (** NOT Class function **)
//-----------------------------------
std::ostream &operator << (std::ostream &os, Dungeon &dun){
    os << dun.width() << " " << dun.height() << endl;
    for (int y = 0; y < dun.height(); y++){
        for (int x = 0; x < dun.width(); x++){
            // os << dun.map_at(x,y) << " ";
            if (dun.map_at(x, y) != Dungeon::WALL && dun.map_at(x, y) != Dungeon::BOUNDARY){
                os << "  ";
            }
            else {
                 os << dun.map_at(x,y) << " ";
            }
        }
        os << endl;
    }
    return os;
}