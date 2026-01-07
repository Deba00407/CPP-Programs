#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <string>
#include <cstdlib>

using namespace std;

class MonsterEscapeGame {
private:
    // Direction vectors for movement
    const vector<int> row_directions = {-1, 1, 0, 0};
    const vector<int> col_directions = {0, 0, 1, -1};
    const vector<char> dir = {'U', 'D', 'R', 'L'};
    
    // Grid properties
    int rows, cols;
    vector<vector<char>> originalGrid;
    vector<vector<char>> currentGrid;
    
    // Game state
    pair<int, int> playerPos;
    pair<int, int> playerStart;
    vector<pair<int, int>> originalMonsterPositions;
    vector<pair<int, int>> currentMonsterPositions;
    
    // Distance and path tracking
    vector<vector<int>> monsterDistance;
    vector<vector<int>> playerDistance;
    vector<vector<char>> parent;
    
    // Optimal solution
    string optimalPath;
    int optimalPathLength;
    pair<int, int> escapePoint;
    
    // Game stats
    int moveCount;
    bool gameOver;
    bool playerWon;

public:
    MonsterEscapeGame(int n, int m) : rows(n), cols(m), moveCount(0), gameOver(false), playerWon(false) {
        originalGrid.resize(rows, vector<char>(cols));
        currentGrid.resize(rows, vector<char>(cols));
        monsterDistance.resize(rows, vector<int>(cols, 1e9));
        playerDistance.resize(rows, vector<int>(cols, 1e9));
        parent.resize(rows, vector<char>(cols, 0));
    }
    
    // Read input and initialize game state
    void readInput() {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                cin >> originalGrid[i][j];
                if (originalGrid[i][j] == 'A') {
                    playerStart = {i, j};
                    playerPos = {i, j};
                } else if (originalGrid[i][j] == 'M') {
                    originalMonsterPositions.push_back({i, j});
                    monsterDistance[i][j] = 0;
                }
            }
        }
        currentGrid = originalGrid;
        currentMonsterPositions = originalMonsterPositions;
    }
    
    // Validate grid coordinates
    bool isValid(int row, int col) const {
        return row >= 0 && row < rows && col >= 0 && col < cols;
    }
    
    // Check if position is on boundary
    bool isBoundary(int row, int col) const {
        if (!isValid(row, col)) return false;
        return row == 0 || row == rows - 1 || col == 0 || col == cols - 1;
    }
    
    // Check if cell is walkable
    bool isWalkable(int row, int col) const {
        return isValid(row, col) && originalGrid[row][col] != '#';
    }
    
    // Compute shortest distances from all monsters using multi-source BFS
    void computeMonsterDistances() {
        queue<pair<int, int>> q;
        
        for (const auto& pos : originalMonsterPositions) {
            q.push(pos);
        }
        
        while (!q.empty()) {
            auto p = q.front();
            q.pop();
            int row = p.first, col = p.second;
            
            for (int k = 0; k < 4; k++) {
                int nr = row + row_directions[k];
                int nc = col + col_directions[k];
                
                if (isWalkable(nr, nc)) {
                    int newDist = monsterDistance[row][col] + 1;
                    if (newDist < monsterDistance[nr][nc]) {
                        monsterDistance[nr][nc] = newDist;
                        q.push({nr, nc});
                    }
                }
            }
        }
    }
    
    // Find escape path for player using BFS
    pair<int, int> findEscapePath() {
        queue<pair<int, int>> q;
        q.push(playerStart);
        playerDistance[playerStart.first][playerStart.second] = 0;
        
        // Check if player starts at boundary
        if (isBoundary(playerStart.first, playerStart.second)) {
            return playerStart;
        }
        
        while (!q.empty()) {
            auto p = q.front();
            q.pop();
            int row = p.first, col = p.second;
            
            for (int k = 0; k < 4; k++) {
                int nr = row + row_directions[k];
                int nc = col + col_directions[k];
                
                if (isWalkable(nr, nc)) {
                    int newPlayerDist = playerDistance[row][col] + 1;
                    
                    // Player can move if they haven't visited yet and reach before monster
                    if (newPlayerDist < playerDistance[nr][nc] && newPlayerDist < monsterDistance[nr][nc]) {
                        playerDistance[nr][nc] = newPlayerDist;
                        parent[nr][nc] = dir[k];
                        q.push({nr, nc});
                        
                        // Check if we reached boundary
                        if (isBoundary(nr, nc)) {
                            return {nr, nc};
                        }
                    }
                }
            }
        }
        
        return {-1, -1};
    }
    
    // Reconstruct path from start to destination
    string reconstructPath(const pair<int, int>& destination) const {
        string path = "";
        int r = destination.first;
        int c = destination.second;
        
        while (make_pair(r, c) != playerStart) {
            char d = parent[r][c];
            path += d;
            
            if (d == 'U') r++;
            else if (d == 'D') r--;
            else if (d == 'L') c++;
            else if (d == 'R') c--;
        }
        
        reverse(path.begin(), path.end());
        return path;
    }
    
    // Compute optimal solution
    bool computeOptimalSolution() {
        computeMonsterDistances();
        escapePoint = findEscapePath();
        
        if (escapePoint == make_pair(-1, -1)) {
            return false;
        }
        
        optimalPath = reconstructPath(escapePoint);
        optimalPathLength = optimalPath.length();
        return true;
    }
    
    // Clear screen (cross-platform)
    void clearScreen() {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }
    
    // Display the current game state
    void displayGrid() {
        clearScreen();
        
        cout << "\n========================================\n";
        cout << "      MONSTER ESCAPE GAME - LIVE!      \n";
        cout << "========================================\n\n";
        
        // Display grid with simple border
        cout << "  +";
        for (int j = 0; j < cols; j++) cout << "-";
        cout << "+\n";
        
        for (int i = 0; i < rows; i++) {
            cout << "  |";
            for (int j = 0; j < cols; j++) {
                cout << currentGrid[i][j];
            }
            cout << "|\n";
        }
        
        cout << "  +";
        for (int j = 0; j < cols; j++) cout << "-";
        cout << "+\n\n";
        
        cout << "  A = You  |  M = Monster  |  # = Wall  |  . = Path\n";
        cout << "  Moves: " << moveCount << "  |  Optimal: " << optimalPathLength << "\n";
        cout << "  Goal: Reach any border!\n\n";
    }
    
    // Move monsters toward player
    void moveMonsters() {
        vector<pair<int, int>> newMonsterPositions;
        
        for (auto& monster : currentMonsterPositions) {
            int mr = monster.first, mc = monster.second;
            int bestDist = abs(playerPos.first - mr) + abs(playerPos.second - mc);
            int bestR = mr, bestC = mc;
            
            // Find best move toward player
            for (int k = 0; k < 4; k++) {
                int nr = mr + row_directions[k];
                int nc = mc + col_directions[k];
                
                if (isWalkable(nr, nc) && make_pair(nr, nc) != playerPos) {
                    int dist = abs(playerPos.first - nr) + abs(playerPos.second - nc);
                    if (dist < bestDist) {
                        bestDist = dist;
                        bestR = nr;
                        bestC = nc;
                    }
                }
            }
            
            newMonsterPositions.push_back({bestR, bestC});
        }
        
        currentMonsterPositions = newMonsterPositions;
    }
    
    // Update grid with current positions
    void updateGrid() {
        // Reset grid to original (keep walls and empty spaces)
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                if (originalGrid[i][j] == '#') {
                    currentGrid[i][j] = '#';
                } else {
                    currentGrid[i][j] = '.';
                }
            }
        }
        
        // Place monsters
        for (const auto& monster : currentMonsterPositions) {
            currentGrid[monster.first][monster.second] = 'M';
        }
        
        // Place player
        currentGrid[playerPos.first][playerPos.second] = 'A';
    }
    
    // Check if player caught by monster
    bool isCaughtByMonster() {
        for (const auto& monster : currentMonsterPositions) {
            if (monster == playerPos) {
                return true;
            }
        }
        return false;
    }
    
    // Process player move
    bool processMove(char move) {
        int newR = playerPos.first;
        int newC = playerPos.second;
        
        // Convert move to direction
        if (move == 'U' || move == 'u') newR--;
        else if (move == 'D' || move == 'd') newR++;
        else if (move == 'L' || move == 'l') newC--;
        else if (move == 'R' || move == 'r') newC++;
        else return false;
        
        // Check if valid move
        if (!isWalkable(newR, newC)) {
            cout << "\n  [X] Invalid move! Can't move there.\n";
            cout << "  Press Enter to continue...";
            cin.ignore();
            cin.get();
            return false;
        }
        
        // Update player position
        playerPos = {newR, newC};
        moveCount++;
        
        // Move monsters
        moveMonsters();
        
        // Update grid
        updateGrid();
        
        // Check game over conditions
        if (isCaughtByMonster()) {
            gameOver = true;
            playerWon = false;
            return true;
        }
        
        if (isBoundary(playerPos.first, playerPos.second)) {
            gameOver = true;
            playerWon = true;
            return true;
        }
        
        return true;
    }
    
    // Display game over message
    void displayGameOver() {
        displayGrid();
        
        cout << "\n";
        if (playerWon) {
            cout << "========================================\n";
            cout << "            *** YOU WON! ***           \n";
            cout << "========================================\n";
            cout << "\n  > You escaped in " << moveCount << " moves!\n";
            cout << "  > Optimal solution: " << optimalPathLength << " moves\n";
            
            if (moveCount == optimalPathLength) {
                cout << "  > PERFECT! You found the optimal path!\n";
            } else if (moveCount <= optimalPathLength + 3) {
                cout << "  > Great job! Very close to optimal!\n";
            } else {
                cout << "  > You made it, but could be more efficient.\n";
            }
        } else {
            cout << "========================================\n";
            cout << "          *** GAME OVER! ***          \n";
            cout << "========================================\n";
            cout << "\n  > You were caught by a monster!\n";
            cout << "  > You survived " << moveCount << " moves.\n";
        }
        
        cout << "\n  Optimal path was: " << optimalPath << "\n\n";
    }
    
    // Main game loop
    void playGame() {
        displayGrid();
        
        while (!gameOver) {
            cout << "  Enter move (U/D/L/R) or Q to quit: ";
            char move;
            cin >> move;
            
            if (move == 'Q' || move == 'q') {
                cout << "\n  Thanks for playing!\n\n";
                return;
            }
            
            if (processMove(move)) {
                displayGrid();
            }
        }
        
        displayGameOver();
    }
    
    // Main solve function
    void solve() {
        readInput();
        
        if (!computeOptimalSolution()) {
            cout << "\n========================================\n";
            cout << "        NO ESCAPE POSSIBLE!            \n";
            cout << "========================================\n";
            cout << "\n  This maze has no solution.\n";
            cout << "  The monsters will always catch you!\n\n";
            return;
        }
        
        cout << "\n========================================\n";
        cout << "      MONSTER ESCAPE GAME - START!     \n";
        cout << "========================================\n\n";
        cout << "  There IS an escape route!\n";
        cout << "  Can you find it before the monsters catch you?\n\n";
        cout << "  Controls:\n";
        cout << "    U - Move Up\n";
        cout << "    D - Move Down\n";
        cout << "    L - Move Left\n";
        cout << "    R - Move Right\n";
        cout << "    Q - Quit\n\n";
        cout << "  Press Enter to start...";
        cin.ignore();
        cin.get();
        
        playGame();
    }
};

int main() {
    int n, m;
    cout << "\nEnter grid dimensions (rows cols): ";
    cin >> n >> m;
    cout << "Enter grid (A=player, M=monster, #=wall, .=path):\n";
    
    MonsterEscapeGame game(n, m);
    game.solve();
    
    return 0;
}