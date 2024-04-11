#include "hw4.h"

int mapping(int num){
    return 7 - num;
}

void spliceString(char *originalString, int start, int end, char *result){

    int length = end - start + 1;

    strncpy(result, originalString + start, length);
    result[length] = '\0';
}

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
            else if (i > 1 && i < 6)
                game->chessboard[i][j] = '.';
        }
    }

    game->capturedCount = 0;
    game->moveCount = 0;
    game->currentPlayer = WHITE_PLAYER;
}

void chessboard_to_fen(char fen[], ChessGame *game) {
    int consecutiveNumber = 0;
    int counter = 0;
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            if (game->chessboard[i][j] == 'r') fen[counter++] = 'r';
            if (game->chessboard[i][j] == 'n') fen[counter++] = 'n';
            if (game->chessboard[i][j] == 'b') fen[counter++] = 'b';
            if (game->chessboard[i][j] == 'q') fen[counter++] = 'q';
            if (game->chessboard[i][j] == 'k') fen[counter++] = 'k';
            if (game->chessboard[i][j] == 'p') fen[counter++] = 'p';

            if (game->chessboard[i][j] == 'R') fen[counter++] = 'R';
            if (game->chessboard[i][j] == 'N') fen[counter++] = 'N';
            if (game->chessboard[i][j] == 'B') fen[counter++] = 'B';
            if (game->chessboard[i][j] == 'Q') fen[counter++] = 'Q';
            if (game->chessboard[i][j] == 'K') fen[counter++] = 'K';
            if (game->chessboard[i][j] == 'P') fen[counter++] = 'P';

            if (game->chessboard[i][j] == '.') consecutiveNumber++;
            if (game->chessboard[i][j] == '.' && j + 1 < 8 && game->chessboard[i][j+1] != '.'){
                fen[counter++] = consecutiveNumber + '0';
                consecutiveNumber = 0;
            }
            else if (game->chessboard[i][j] == '.' && (j + 1 == 8) ){
                fen[counter++] = consecutiveNumber + '0';
                consecutiveNumber = 0;
            }

        }
        if (i < 7)
            fen[counter++] = '/';
    }

    fen[counter++] = ' ';
    if (game->currentPlayer == BLACK_PLAYER){
        fen[counter++] = 'b';
    } 
    else{
        fen[counter++] = 'w';
    }

    fen[counter] = '\0';
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
            if (game->chessboard[dest_row][i] != '.') return false;
        }
    }
    else if (src_col == dest_col){ // Moving vertically  
        for ((dest_row > src_row) ? (i = src_row + 1) : (i = src_row - 1) ; (dest_row > src_row) ? (i < dest_row) : (i > dest_row); (dest_row > src_row) ? (i++) : (i--)) {
            if (game->chessboard[i][dest_col] != '.') return false;
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
    if (piece == 'p' || piece == 'P')
        return is_valid_pawn_move(piece, src_row, src_col, dest_row, dest_col, game);
    else if (piece == 'n' || piece == 'N')
        return is_valid_knight_move(src_row, src_col, dest_row, dest_col);
    else if (piece == 'r' || piece == 'R')
        return is_valid_rook_move(src_row, src_col, dest_row, dest_col, game);
    else if (piece == 'b' || piece == 'B')
        return is_valid_bishop_move(src_row, src_col, dest_row, dest_col, game);
    else if (piece == 'q' || piece == 'Q')
        return is_valid_queen_move(src_row, src_col, dest_row, dest_col, game);
    else if (piece == 'k' || piece == 'K')
        return is_valid_king_move(src_row, src_col, dest_row, dest_col);
    else
        return false;
}

void fen_to_chessboard(const char *fen, ChessGame *game) {
    int row = 0;
    int col = 0;

    for(int i = 0; i < (int)strlen(fen)-2; i++){
        if (isalpha(fen[i])){
            if (fen[i] == 'r') game->chessboard[row][col] = 'r';
            if (fen[i] == 'n') game->chessboard[row][col] = 'n';
            if (fen[i] == 'b') game->chessboard[row][col] = 'b';
            if (fen[i] == 'q') game->chessboard[row][col] = 'q';
            if (fen[i] == 'k') game->chessboard[row][col] = 'k';
            if (fen[i] == 'p') game->chessboard[row][col] = 'p';
            
            if (fen[i] == 'R') game->chessboard[row][col] = 'R';
            if (fen[i] == 'N') game->chessboard[row][col] = 'N';
            if (fen[i] == 'B') game->chessboard[row][col] = 'B';
            if (fen[i] == 'Q') game->chessboard[row][col] = 'Q';
            if (fen[i] == 'K') game->chessboard[row][col] = 'K';
            if (fen[i] == 'P') game->chessboard[row][col] = 'P';
            col++;
        }

        if (fen[i] >= '1' && fen[i] <= '8'){
            int numSpaces = fen[i] - '0';
            for(int j = 0; j < numSpaces; j++){
                game->chessboard[row][col] = '.';
                col++;
            }
        }

        if (fen[i] == '/'){
            row++;
            col = 0;
        }
    }

    if (fen[strlen(fen) - 1] == 'b')
        game->currentPlayer = BLACK_PLAYER;
    else
        game->currentPlayer = WHITE_PLAYER;
}

int parse_move(const char *move, ChessMove *parsed_move) {
    int length = strlen(move);

    if (length < 4 || length > 5) return PARSE_MOVE_INVALID_FORMAT;

    char copyOfMove[length + 1];
    strcpy(copyOfMove, move);
    copyOfMove[length] = '\0';
    char rowLetter1 = copyOfMove[0];
    int col1 = copyOfMove[1] - '0';
    char rowLetter2 = copyOfMove[2];
    int col2 = copyOfMove[3] - '0';

    if ((rowLetter1 < 'a' || rowLetter1 > 'h') || (rowLetter2 < 'a' || rowLetter2 > 'h')) return PARSE_MOVE_INVALID_FORMAT;   
    if ((col1 < 1 || col1 > 8) || (col2 < 1 || col2 > 8)) return PARSE_MOVE_OUT_OF_BOUNDS;
    
    char promotionType = '\0';

    if (length == 5){
        promotionType = copyOfMove[4];
        if (col2 == 8 && col1 < 7) return PARSE_MOVE_INVALID_DESTINATION;
        else if (col2 == 1 && col1 > 2) return PARSE_MOVE_INVALID_DESTINATION;
        else if ((col2 != 1 && col2 != 8)) return PARSE_MOVE_INVALID_DESTINATION;
        if (promotionType != 'q' && promotionType != 'r' && promotionType != 'b' && promotionType != 'n')return PARSE_MOVE_INVALID_PROMOTION;
    }

    parsed_move->startSquare[0] = rowLetter1;
    parsed_move->startSquare[1] = col1 + '0';
    parsed_move->startSquare[2] = '\0';

    parsed_move->endSquare[0] = rowLetter2;
    parsed_move->endSquare[1] = col2 + '0';
    
    if (length == 5){
        parsed_move->endSquare[2] = promotionType;
        parsed_move->endSquare[3] = '\0';
    }
    else{
        parsed_move->endSquare[2] = '\0';
    }
    return 0;
}

int make_move(ChessGame *game, ChessMove *move, bool is_client, bool validate_move) {
    int startColumn = move->startSquare[0] - 'a';
    int startRow = move->startSquare[1] - '0' - 1;
    startRow = mapping(startRow);

    int endColumn = move->endSquare[0] - 'a';
    int endRow = move->endSquare[1] - '0' - 1;
    endRow = mapping(endRow);

    if (validate_move){
        if (is_client == true && game->currentPlayer == BLACK_PLAYER)
            return MOVE_OUT_OF_TURN;
        if (is_client == false && game->currentPlayer == WHITE_PLAYER)
            return MOVE_OUT_OF_TURN;
        if (game->chessboard[startRow][startColumn] == '.')
            return MOVE_NOTHING;
        if (is_client == true && islower(game->chessboard[startRow][startColumn]) > 0) // White Player is going, but the start location is a black piece
            return MOVE_WRONG_COLOR;
        if (is_client == false && isupper(game->chessboard[startRow][startColumn]) > 0)  // Black player is going, but the start location is a white piece
            return MOVE_WRONG_COLOR;
        if (is_client == true && isupper(game->chessboard[endRow][endColumn]) > 0) // White Player is going, but the end location is a white piece
            return MOVE_SUS;
        if (is_client == false && islower(game->chessboard[endRow][endColumn]) > 0) // Black player is going, but the end location is a black piece
            return MOVE_SUS;
        if (strlen(move->endSquare) == 3 && (game->chessboard[startRow][startColumn] != 'p' && game->chessboard[startRow][startColumn] != 'P'))  // Means that there is a promotion 
            return MOVE_NOT_A_PAWN;
        if (is_client == true && strlen(move->endSquare) == 2 && endRow == 7 && game->chessboard[startRow][startColumn] == 'P' )  // White player is going, going to the 8th row with a pawn, but no promotion symbol 
            return MOVE_MISSING_PROMOTION;
        if (is_client == false && strlen(move->endSquare) == 2 && endRow == 0 && game->chessboard[startRow][startColumn] == 'p') // Black player is going, going to the 1st row with a pawn, but no promotion symbol
            return MOVE_MISSING_PROMOTION;
        if (is_valid_move(game->chessboard[startRow][startColumn], startRow, startColumn, endRow, endColumn, game) == false)
            return MOVE_WRONG;
    }      

    if (game->chessboard[startRow][startColumn] == 'p' && endRow == 7){ // Black pawn going to the 8th row
        if (game->chessboard[endRow][endColumn] == '.'){ // End location is empty
            game->chessboard[endRow][endColumn] = 'q';
        }
        else{ // End location is not empty
            game->capturedPieces[game->capturedCount] = game->chessboard[endRow][endColumn];
            game->capturedCount++;
            game->chessboard[endRow][endColumn] = 'q';
        }
        game->chessboard[startRow][startColumn] = '.';
    } 
    else if (game->chessboard[startRow][startColumn] == 'P'  && endRow == 0) { // White pawn going to the 1st row
        if (game->chessboard[endRow][endColumn] == '.'){ // End location is empty
            game->chessboard[endRow][endColumn] = 'Q';
        }
        else{ // End location is not empty
            game->capturedPieces[game->capturedCount] = game->chessboard[endRow][endColumn];
            game->capturedCount++;
            game->chessboard[endRow][endColumn] = 'Q';
        }
        game->chessboard[startRow][startColumn] = '.';
    }
    else if (game->chessboard[endRow][endColumn] == '.'){ // Moving to an empty location
        game->chessboard[endRow][endColumn] = game->chessboard[startRow][startColumn];
        game->chessboard[startRow][startColumn] = '.';
    }
    else if (isupper(game->chessboard[startRow][startColumn]) > 0 && islower(game->chessboard[endRow][endColumn]) > 0){   // Starting Piece is White, Ending Piece is Black
        game->capturedPieces[game->capturedCount] = game->chessboard[endRow][endColumn]; 
        game->capturedCount++;
        game->chessboard[endRow][endColumn] = game->chessboard[startRow][startColumn];
        game->chessboard[startRow][startColumn] = '.';
    }
    else if (islower(game->chessboard[startRow][startColumn]) > 0 && isupper(game->chessboard[endRow][endColumn]) > 0){ // Starting Piece is Black, Ending Piece is White
        game->capturedPieces[game->capturedCount] = game->chessboard[endRow][endColumn];
        game->capturedCount++;
        game->chessboard[endRow][endColumn] = game->chessboard[startRow][startColumn];
        game->chessboard[startRow][startColumn] = '.';
        game->capturedCount++;
    }
    
    game->moves[game->moveCount] = *move;
    game->moveCount++;

    if (is_client == true) game->currentPlayer = BLACK_PLAYER;
    else game->currentPlayer = WHITE_PLAYER;


    return 0;
}


int send_command(ChessGame *game, const char *message, int socketfd, bool is_client) {
   
    char copyMessage[strlen(message) + 1];
    strncpy(copyMessage, message, strlen(message));
    copyMessage[strlen(message)] = '\0';
    int start = 0;
    int end = 0;
    
    char *ptr = copyMessage;

    while (*ptr != '\0' && *ptr != ' '){
        ptr++;
        end++;
    }
    
    end--;
    char command[end - start + 2];
    spliceString(copyMessage, start, end, command);
    if (strcmp(command, "/forfeit") == 0){
        printf("%s\n", message);
        send(socketfd, message, strlen(message), 0);
        return COMMAND_FORFEIT;
    }

    if (strcmp(command, "/chessboard")  == 0){
        display_chessboard(game);
        return COMMAND_DISPLAY;
    }

    if (strcmp(command, "/import") != 0 && strcmp(command, "/move") != 0 && strcmp(command, "/save") != 0&& strcmp(command, "/load") != 0)
        return COMMAND_UNKNOWN;

    // Rest of the ones have arguments

    end = end + 2;
    start = end;
    end = strlen(message) - 1;
    char argument[end - start + 2];
    spliceString(copyMessage, start, end, argument);

    if (strcmp(command, "/move") == 0){
        
        ChessMove parsed_move;
        if (parse_move(argument, &parsed_move) == 0)
            if (make_move(game, &parsed_move, is_client, 1) == 0){
                send(socketfd, message, strlen(message), 0);
                return COMMAND_MOVE;
            }
            else 
                return COMMAND_ERROR;
        else
            return COMMAND_ERROR;
            
    }

    if (strcmp(command, "/import") == 0){
        if (is_client == false){
            fen_to_chessboard(argument, game);
            send(socketfd, message, sizeof(message), is_client);
            return COMMAND_IMPORT;
        }
    }


    if (strcmp(command, "/save") == 0){
        if (save_game(game, argument, "./game_database.txt") == 0) 
            return COMMAND_SAVE;
        else
            return COMMAND_ERROR;
    }

    if (strcmp(command, "/load") == 0){
        ptr = copyMessage;
        ptr = ptr + start;

        end = start;
        
        while (*ptr != ' '){
            ptr++;
            end++;
        }
        end--;

        char name[end - start + 2];
        spliceString(copyMessage, start, end, name);

        end = end + 2;
        start = end;
        ptr++;
        while (*ptr != '\0'){
            ptr++;
            end++;
        }

        char saveString[end - start + 2];
        spliceString(copyMessage, start, end, saveString);

        int saveNumber = (int)(*saveString) - '0';


        if (load_game(game, name, "./game_database.txt", saveNumber) == 0)
            return COMMAND_LOAD;
        else
            return COMMAND_ERROR;
    }


    return COMMAND_UNKNOWN;
}

int receive_command(ChessGame *game, const char *message, int socketfd, bool is_client) {
    char copyMessage[strlen(message) + 1];
    strncpy(copyMessage, message, strlen(message));
    copyMessage[strlen(message)] = '\0';
    int start = 0;
    int end = 0;
    
    char *ptr = copyMessage;

    while (*ptr != '\0' && *ptr != ' '){
        ptr++;
        end++;
    }
    end--;
    char command[end - start + 2];
    spliceString(copyMessage, start, end, command);
    if (strcmp(command, "/forfeit") == 0){
        close(socketfd);
        return COMMAND_FORFEIT;
    }

    if (strcmp(command, "/move") != 0 && strcmp(command, "/import") != 0 && strcmp(command, "/load") != 0)
        return -1;

    end = end + 2;
    start = end;
    end = strlen(message) - 1;
    char argument[end - start + 2];
    spliceString(copyMessage, start, end, argument);

     if (strcmp(command, "/move") == 0){
        ChessMove parsed_move;
        if (parse_move(argument, &parsed_move) == 0){
            make_move(game, &parsed_move, is_client, false);
            return COMMAND_MOVE;
        }
        else
            return COMMAND_ERROR;
     }

     if (strcmp(command, "/import") == 0 && is_client == true){
        fen_to_chessboard(argument, game);
        return COMMAND_IMPORT;
     }

     if (strcmp(command, "/load") == 0){

        ptr = copyMessage;
        ptr = ptr + start;

        end = start;
        
        while (*ptr != ' '){
            ptr++;
            end++;
        }
        end--;

        char name[end - start + 2];
        spliceString(copyMessage, start, end, name);

        end = end + 2;
        start = end;
        ptr++;
        while (*ptr != '\0'){
            ptr++;
            end++;
        }

        char saveString[end - start + 2];
        spliceString(copyMessage, start, end, saveString);

        int saveNumber = (int)(*saveString) - '0';

        if (saveNumber < 1)
            return COMMAND_ERROR;

        if (load_game(game, name, "./game_database.txt", saveNumber) == 0)
            return COMMAND_LOAD;
        else
            return COMMAND_ERROR;
     }

    return -1;
}

int save_game(ChessGame *game, const char *username, const char *db_filename) {
    if (strlen(username) == 0)
        return -1;

    for(int i = 0; i < (int)strlen(username); i++){
        if (username[i] == ' ')
            return -1;
    }

    FILE *filePtr = fopen(db_filename, "a");

    if (filePtr == NULL){
        fclose(filePtr);
        return -1;
    }


    char fenString[BUFFER_SIZE];
    chessboard_to_fen(fenString, game);

    char fullString[BUFFER_SIZE];
    strcpy(fullString, username);
    strcat(fullString, ":");
    strcat(fullString, fenString);

    fprintf(filePtr, "%s\n", fullString);

    
    fclose(filePtr);
    return 0;
}

int load_game(ChessGame *game, const char *username, const char *db_filename, int save_number) {
    if (save_number == 0)
        return -1;

    FILE *read = fopen(db_filename, "r");

    char fenString[BUFFER_SIZE];
    int numGamesSaved = 0;
    while (fgets(fenString, sizeof(fenString), read) != NULL){

        char *ptr = fenString;
        int start = 0;
        int end = 0;
        for(; *ptr != ':'; ptr++){
            end++;
        }
        end--;
        char splicedUsername[end - start + 2];
        spliceString(fenString, start, end, splicedUsername);

        if (strcmp(splicedUsername, username) == 0)
            numGamesSaved++;
    }

    if (numGamesSaved < save_number){
        return -1;
    }

    rewind(read);

    while (fgets(fenString, sizeof(fenString), read) != NULL){

        char *ptr = fenString;
        int start = 0;
        int end = 0;
        for(; *ptr != ':'; ptr++){
            end++;
        }
        end--;
        char splicedUsername[end - start + 2];
        spliceString(fenString, start, end, splicedUsername);

        if (strcmp(splicedUsername, username) == 0)
            numGamesSaved++;

        if (numGamesSaved == save_number){
            break;
        }
    }

    char *fenStringNoName = fenString;
    while (*fenStringNoName != ':'){
        fenStringNoName++;
    }
    fenStringNoName++;
    fen_to_chessboard(fenStringNoName, game);


    fclose(read);
    return 0;
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
