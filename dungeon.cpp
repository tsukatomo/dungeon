#include <iostream>
#include <string>
#include "dungeon.hpp"

using namespace std;


/*
    [NOTE] color escape sequence
    \x1b[?m
    ?=(letter/background)
    30/40...black
    31/41...red
    32/42...green
    33/43...yellow
    34/44...blue
    35/45...magenta
    36/46...cyan
    37/47...white
    ?=0...reset all
*/
const string DEFCOLOR_WALL = "\x1b[40m";
const string DEFCOLOR_BOUNDARY = "\x1b[40m";
const string DEFCOLOR_ROOM = "\x1b[44m";
const string DEFCOLOR_AISLE = "\x1b[45m";
const string DEFCOLOR_RESET = "\x1b[0m";

void rich_output(Dungeon dun){
    for (int y = 0; y < dun.height(); y++){
        for (int x = 0; x < dun.width(); x++){
            switch (dun.map_at(x, y)){
            case Dungeon::WALL :
                cout << DEFCOLOR_WALL;
                break;
            case Dungeon::BOUNDARY :
                cout << DEFCOLOR_BOUNDARY;
                break;
            case Dungeon::ROOM :
                cout << DEFCOLOR_ROOM;
                break;
            case Dungeon::AISLE :
                cout << DEFCOLOR_AISLE;
                break;
            default:
                cout << DEFCOLOR_RESET;
                break;
            }
            cout << "  ";
        }
        cout << DEFCOLOR_RESET << endl;
    }
}

//-----------------
   /////////////// 
  //  M A I N  //
 ///////////////
//-----------------
int main(){
    Dungeon dun;
    int width = 48, height = 32, n_room = 20;
    bool errchk;
    char key_command = 's';
    while (key_command == 's') {
        // input params
        errchk = true;
        while (errchk){
            errchk = false;
            cout << "パラメータ入力: 幅 高さ 最大部屋数 > ";
            cin >> width >> height >> n_room;
            if (width < 7){
                cerr << "エラー: 幅が小さすぎます．" << endl;
                errchk = true;
            }
            if (height < 7){
                cerr << "エラー: 高さが小さすぎます．" << endl;
                errchk = true;
            }
            if (n_room <= 0){
                cerr << "エラー: 部屋数は正の整数を入力してください．" << endl;
                errchk = true;
            }
        }
        // draw to console
        key_command = 'r';
        while (key_command == 'r') {
            cout << "\x1b[2J";
            cout << width << "x" << height << ", 最大部屋数=" << n_room << endl;
            dun.make_dungeon(width, height, n_room);
            rich_output(dun);
            cout << "[r] もう一度 / [s] パラメータ変更 / [その他] やめる > ";
            cin >> key_command;
        }
    }
    return 0;
}
