#include "hw4.h"

void initialize_game(ChessGame *game) {
    game->chessboard[0][0] = 'r';
    game->chessboard[0][1] = 'n';
    game->chessboard[0][2] = 'b';
    game->chessboard[0][3] = 'q';
    game->chessboard[0][4] = 'k';
    game->chessboard[0][5] = 'b';
    game->chessboard[0][6] = 'n';
    game->chessboard[0][7] = 'r';

    game->chessboard[7][0] = 'R';
    game->chessboard[7][1] = 'N';
    game->chessboard[7][2] = 'B';
    game->chessboard[7][3] = 'Q';
    game->chessboard[7][4] = 'K';
    game->chessboard[7][5] = 'B';
    game->chessboard[7][6] = 'N';
    game->chessboard[7][7] = 'R';


    for(int i = 1; i < 7; i++){
        for (int j = 0; j < 8; j++){
            if (i == 1)
                game->chessboard[i][j] = 'p';
            else if (i == 6)
                game->chessboard[i][j] = 'P';
            else if (i > 1 && i < 6){
                game->chessboard[i][j] = '.';
            }
        }
    }

    game->capturedCount = 0;
    game->moveCount = 0;
    game->currentPlayer = WHITE_PLAYER;
}

void chessboard_to_fen(char fen[], ChessGame *game) {
    (void)fen;
    (void)game;
}
 
bool is_valid_pawn_move(char piece, int src_row, int src_col, int dest_row, int dest_col, ChessGame *game) {
    if (piece == 'P'){ // white pieces
        if (src_row == 6 && dest_row == 4 && dest_col == src_col )   // Trying to move 2 places forwards 
                if (game->chessboard[5][dest_col] == '.' && game->chessboard[4][src_col] == '.') return true;
        if (dest_row == src_row - 1 && dest_col == src_col)         // General case moving 1 place forward
            if (game->chessboard[dest_row][dest_col] == '.') return true;
        if (dest_row == src_row - 1 && (dest_col == src_col + 1 || dest_col == src_col - 1)){
            if (dest_col == src_col + 1)
                if (game->chessboard[dest_row][dest_col] != '.') return true;
            if (dest_col == src_col - 1)
                if (game->chessboard[dest_row][dest_col] != '.') return true;
        }
    }

    if (piece == 'p'){ // black pieces
        if (src_row == 1 && dest_row == 3 && dest_col == src_col)    // Trying to move 2 places forwards
            if (game->chessboard[2][dest_col] == '.' && game->chessboard[3][src_col] == '.') return true;
        if (dest_row == src_row + 1 && dest_col == src_col)         // General case moving 1 place forward
            if (game->chessboard[dest_row][dest_col] == '.') return true;
        if (dest_row == src_row + 1 && (dest_col == src_col + 1 || dest_col == src_col - 1)){   // Moving diagonal
            if (dest_col == src_col + 1)
                if (game->chessboard[dest_row][dest_col] != '.') return true;
            if (dest_col == src_col - 1)
                if (game->chessboard[dest_row][dest_col] != '.') return true;      
        }
    }
    return false;
}

bool is_valid_rook_move(int src_row, int src_col, int dest_row, int dest_col, ChessGame *game) {

    int i;
    if (src_row == dest_row){ // Moving horizontally

        for ((dest_col > src_col) ? (i = src_col + 1) : (i = src_col - 1) ; (dest_col > src_col) ? (i < dest_col) : (i > dest_col); (dest_col > src_col) ? (i++) : (i--)){
            if (game->chessboard[dest_row][i] != '.')
                return false;
        }
    }
    else if (src_col == dest_col){ // Moving vertically
        
        for ((dest_row > src_row) ? (i = src_row + 1) : (i = src_row - 1) ; (dest_row > src_row) ? (i < dest_row) : (i > dest_row); (dest_row > src_row) ? (i++) : (i--)) {
            if (game->chessboard[i][dest_col] != '.')
                return false;
        }
    }
    else{ // Not moving horizontally or vertically
        return false;
    }

    char destination_piece = game->chessboard[dest_row][dest_col];
    char source_piece = game->chessboard[src_row][src_col];

    if ((islower(destination_piece) && islower(source_piece)) || (isupper(destination_piece) && isupper(source_piece))) // Same color at destination piece and source piece
        return false;


    return true;
}

bool is_valid_knight_move(int src_row, int src_col, int dest_row, int dest_col) {
    int rowDifference = abs(dest_row - src_row);
    int colDifference = abs(dest_col - src_col);

    return ((rowDifference == 1 && colDifference == 2 ) || (rowDifference == 2 && colDifference == 1));
}

bool is_valid_bishop_move(int src_row, int src_col, int dest_row, int dest_col, ChessGame *game) {
    int rowDifference = abs(dest_row - src_row);
    int colDifference = abs(dest_col - src_col);

    if (rowDifference != colDifference)  // Difference must be the same in order for diagonal movement
        return false;

    int rowDirection = (dest_row > src_row) ? 1 : -1; 
    int colDirection = (dest_col > src_col) ? 1 : -1;

    int startRow = src_row + rowDirection;
    int startCol = src_col + colDirection;

    while (startRow != dest_row && startCol != dest_col){
        if (game->chessboard[startRow][startCol] != '.')
            return false;
        startRow += rowDirection;
        startCol += colDirection;
    }

    char destination_piece = game->chessboard[dest_row][dest_col];
    char source_piece = game->chessboard[src_row][src_col];

    if ((islower(destination_piece) && islower(source_piece)) || (isupper(destination_piece) && isupper(source_piece))) // Same color at destination piece and source piece
        return false;

    return true;
}

bool is_valid_queen_move(int src_row, int src_col, int dest_row, int dest_col, ChessGame *game) {
    int rowDifference = abs(dest_row - src_row);
    int colDifference = abs(dest_col - src_col);

    if ((src_row == dest_row) || (src_col == dest_col) || (rowDifference == colDifference)){
        if (is_valid_rook_move(src_row, src_col, dest_row, dest_col, game))
            return true;
        if (is_valid_bishop_move(src_row, src_col, dest_row, dest_col, game))
            return true;
    }
    
    return false;
}

bool is_valid_king_move(int src_row, int src_col, int dest_row, int dest_col) {
    int rowDifference = abs(dest_row - src_row);
    int colDifference = abs(dest_col - src_col);

    return (rowDifference <= 1 && colDifference <= 1);
}

bool is_valid_move(char piece, int src_row, int src_col, int dest_row, int dest_col, ChessGame *game) {
    (void)piece;
    (void)src_row;
    (void)src_col;
    (void)dest_row;
    (void)dest_col;
    (void)game;
    return false;
}

void fen_to_chessboard(const char *fen, ChessGame *game) {
    (void)fen;
    (void)game;
}

int parse_move(const char *move, ChessMove *parsed_move) {
    (void)move;
    (void)parsed_move;
    return -999;
}

int make_move(ChessGame *game, ChessMove *move, bool is_client, bool validate_move) {
    (void)game;
    (void)move;
    (void)is_client;
    (void)validate_move;
    return -999;
}

int send_command(ChessGame *game, const char *message, int socketfd, bool is_client) {
    (void)game;
    (void)message;
    (void)socketfd;
    (void)is_client;
    return -999;
}

int receive_command(ChessGame *game, const char *message, int socketfd, bool is_client) {
    (void)game;
    (void)message;
    (void)socketfd;
    (void)is_client;
    return -999;
}

int save_game(ChessGame *game, const char *username, const char *db_filename) {
    (void)game;
    (void)username;
    (void)db_filename;
    return -999;
}

int load_game(ChessGame *game, const char *username, const char *db_filename, int save_number) {
    (void)game;
    (void)username;
    (void)db_filename;
    (void)save_number;
    return -999;
}

void display_chessboard(ChessGame *game) {
    printf("\nChessboard:\n");
    printf("  a b c d e f g h\n");
    for (int i = 0; i < 8; i++) {
        printf("%d ", 8 - i);
        for (int j = 0; j < 8; j++) {
            printf("%c ", game->chessboard[i][j]);
        }
        printf("%d\n", 8 - i);
    }
    printf("  a b c d e f g h\n");
}
