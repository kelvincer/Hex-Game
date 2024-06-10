#include <iostream>
#include <array>
#include <vector>
#include <unordered_map>
#include <stack>
#include <algorithm>
#include <list>
#include <set>

using namespace std;

constexpr int board_size = 7; // Board size
constexpr int R_VALUE = 0; // Value for red on the board array
constexpr int B_VALUE = 1; // Value for blue on the board array

// Class to represent a game hexagon
struct Node {
    // Positions
    int x;
    int y;

    Node(int x, int y): x{x}, y{y} {
    }

    friend ostream &operator<<(ostream &out, const Node node) {
        out << "(" << node.x << ", " << node.y << ")";
        return out;
    }

    bool operator==(const Node &n1) const {
        return x == n1.x && y == n1.y;
    }
};

// Hash for Node class
template<>
struct std::hash<Node> {
    std::size_t operator()(const Node &node) const noexcept {
        std::size_t h1 = std::hash<int>{}(node.x);
        std::size_t h2 = std::hash<int>{}(node.y);
        return h1 ^ (h2 << 1);
    }
};

// Method declaration to print the board
void print_board(array<array<int, board_size>, board_size> &, int, int, char c = '\0');

// Method declaration for creating the connections between nodes
void create_connections(unordered_map<Node, vector<Node> > &);

// Method that returns the winner of the game
pair<string, bool> get_winner(unordered_map<Node, vector<Node> > &,
                              array<array<int, board_size>, board_size> &);

// Method that returns the chosen nodes by red player in the row y
vector<Node> get_all_nodes_of_red_from_same_row(vector<Node> &nodes, int y);

// Method that returns the chosen nodes by blue player in the column x
vector<Node> get_all_nodes_of_blue_from_same_column(vector<Node> &nodes, int x);

// Method that returns if red player has won
bool is_red_nodes_a_complete_chain(unordered_map<Node, vector<Node> > &connections, vector<Node> &player_nodes);

// Method that returns if blue player has won
bool is_blue_nodes_a_complete_chain(unordered_map<Node, vector<Node> > &connections, vector<Node> &player_nodes);

// Method that returns if blue player has built a connected path from x = 0 to x = board_size-1
bool has_completed_blue_path(unordered_map<Node, vector<Node> > &connections, vector<Node> nodes);

// Method that returns if red player has built a connected path from y = 0 to y = board_size-1
bool has_completed_red_path(unordered_map<Node, vector<Node> > &connections, vector<Node> nodes);

// Method that returns if two nodes are connected
bool is_nodes_connected(unordered_map<Node, vector<Node> > &connections, Node n1, Node n2);

// Main function
int main() {
    // Create board
    array<array<int, board_size>, board_size> board{};
    for (int i = 0; i < board_size; i++) {
        for (int j = 0; j < board_size; j++) {
            board[i][j] = -1;
        }
    }

    // Select color
    string player_one, player_two;
    cout << "Welcome to Hex game. Choose your color Red(enter R) or Blue(enter B): ";
    cin >> player_one;
    if (player_one == "R") {
        player_two = "B";
    } else {
        player_two = "R";
    }
    cout << "Player one: " << player_one << ", player two: " << player_two << endl;

    int input_x, input_y;

    bool red_turn = player_one == "R";

    // Print board
    print_board(board, -1, -1);

    unordered_map<Node, vector<Node> > connections;

    // Create connections
    create_connections(connections);

    // Loop of the game
    while (true) {
        char s;
        if (red_turn) {
            cout << "Red(R) enter a position (i j): ";
        } else {
            cout << "Blue(B) enter a position (i j): ";
        }
        cin >> input_x >> input_y;
        s = red_turn ? 'X' : 'O';
        int x = input_x - 1, y = input_y - 1;

        if (board[x][y] != -1 || x < 0 || x > board_size || y < 0 || y > board_size) {
            cout << endl << "This position is invalid!" << endl;
            continue;
        }

        print_board(board, x, y, s);
        auto result = get_winner(connections, board);
        if (result.second) {
            cout << "Winner: " << result.first << endl;
            break;;
        }
        cout << endl;
        red_turn = !red_turn;
    }
}

// Method definition to print the board
void print_board(array<array<int, board_size>, board_size> &board, const int x, int y, char s) {
    cout << endl;
    board[x][y] = s == 'X' ? R_VALUE : B_VALUE;

    int h = 0;
    int l = 1;
    for (int j = 0; j < board_size; j++) {
        // Print first node
        int i = 0;
        if (board[i][j] == -1) {
            cout << "•";
        } else if (board[i][j] == 0) {
            cout << "X";
        } else {
            cout << "O";
        }
        // Print next rows of nodes
        i++;
        for (; i < board_size; i++) {
            if (board[i][j] == -1) {
                cout << " - " << "•";
            } else if (board[i][j] == 0) {
                cout << " - " << "X";
            } else {
                cout << " - " << "O";
            }
        }
        cout << endl;
        for (int j = 0; j < h + 1; j = j + 1) {
            cout << " ";
        }
        if (j == board_size - 1) {
            break;
        }

        // Print connections between nodes
        h += 2;
        cout << "\\ ";
        for (int i = 0; i < board_size * 2 - 2; i++) {
            cout << (i % 2 == 0 ? "/ " : "\\ ");
        }
        cout << endl;
        for (int j = 0; j < l + 1; j = j + 1)
            cout << " ";
        l += 2;
    }
    cout << endl;
}

// Method definition for creating the connections between nodes like a map
void create_connections(unordered_map<Node, vector<Node> > &connections) {
    // Connections for node: (0,0)
    connections[Node(0, 0)].push_back(Node(0, 1));
    connections[Node(0, 0)].push_back(Node(1, 0));

    // Connections for node: (board_size-1, board_size - 1)
    connections[Node(board_size - 1, board_size - 1)].push_back(Node(board_size - 1, board_size - 2));
    connections[Node(board_size - 1, board_size - 1)].push_back(Node(board_size - 2, board_size - 1));

    // Connections for nodes: (i, 0)
    for (int i = 1; i < board_size - 1; i++) {
        connections[Node(i, 0)].push_back(Node(i - 1, 0));
        connections[Node(i, 0)].push_back(Node(i - 1, 1));
        connections[Node(i, 0)].push_back(Node(i, 1));
        connections[Node(i, 0)].push_back(Node(i + 1, 0));
    }

    // Connections for nodes: (board_size-1,0)
    connections[Node(board_size - 1, 0)].push_back(Node(board_size - 2, 0));
    connections[Node(board_size - 1, 0)].push_back(Node(board_size - 2, 1));
    connections[Node(board_size - 1, 0)].push_back(Node(board_size - 1, 1));

    // Connections for node: (0, board_size-1)
    connections[Node(0, board_size - 1)].push_back(Node(0, board_size - 2));
    connections[Node(0, board_size - 1)].push_back(Node(1, board_size - 2));
    connections[Node(0, board_size - 1)].push_back(Node(1, board_size - 1));

    // Connections for nodes: (i, board_size-1)
    for (int i = 1; i < board_size - 1; i++) {
        connections[Node(i, board_size - 1)].push_back(Node(i - 1, board_size - 1));
        connections[Node(i, board_size - 1)].push_back(Node(i, board_size - 2));
        connections[Node(i, board_size - 1)].push_back(Node(i + 1, board_size - 2));
        connections[Node(i, board_size - 1)].push_back(Node(i + 1, board_size - 1));
    }

    // Connections for nodes: (0, j)
    for (int j = 1; j < board_size - 1; j++) {
        connections[Node(0, j)].push_back(Node(0, j - 1));
        connections[Node(0, j)].push_back(Node(1, j - 1));
        connections[Node(0, j)].push_back(Node(1, j));
        connections[Node(0, j)].push_back(Node(0, j + 1));
    }

    // Connections for nodes: (board_size-1, j)
    for (int j = 1; j < board_size - 1; j++) {
        connections[Node(board_size - 1, j)].push_back(Node(board_size - 1, j - 1));
        connections[Node(board_size - 1, j)].push_back(Node(board_size - 2, j));
        connections[Node(board_size - 1, j)].push_back(Node(board_size - 2, j + 1));
        connections[Node(board_size - 1, j)].push_back(Node(board_size - 1, j + 1));
    }

    // Connections for nodes: (i, j)
    for (int i = 1; i < board_size - 1; i++) {
        for (int j = 1; j < board_size - 1; j++) {
            connections[Node(i, j)].push_back(Node(i - 1, j));
            connections[Node(i, j)].push_back(Node(i, j - 1));
            connections[Node(i, j)].push_back(Node(i + 1, j - 1));
            connections[Node(i, j)].push_back(Node(i + 1, j));
            connections[Node(i, j)].push_back(Node(i, j + 1));
            connections[Node(i, j)].push_back(Node(i - 1, j + 1));
        }
    }
}

// Method that returns the winner of the game
pair<string, bool> get_winner(unordered_map<Node, vector<Node> > &connections,
                              array<array<int, board_size>, board_size> &board) {
    // Create a vector of nodes for each player
    vector<Node> red_nodes;
    vector<Node> blue_nodes;
    for (int i = 0; i < board_size; i++) {
        for (int j = 0; j < board_size; j++) {
            if (board[i][j] == R_VALUE) {
                red_nodes.push_back(Node(i, j));
            }
            if (board[i][j] == B_VALUE) {
                blue_nodes.push_back(Node(i, j));
            }
        }
    }

    // Sort the vector of nodes
    sort(red_nodes.begin(), red_nodes.end(), [](Node n1, Node n2) { return n1.y < n2.y; });
    sort(blue_nodes.begin(), blue_nodes.end(), [](Node n1, Node n2) { return n1.x < n2.x; });

    // Compute if there is a winner
    auto is_red_nodes_connected = is_red_nodes_a_complete_chain(connections, red_nodes);
    auto is_blue_nodes_connected = is_blue_nodes_a_complete_chain(connections, blue_nodes);

    // Return the winner
    if (is_red_nodes_connected) {
        return make_pair("Red", true);
    }
    if (is_blue_nodes_connected) {
        return make_pair("Blue", true);
    }

    return make_pair("", false);
}

// Method that returns if red player has won
bool is_red_nodes_a_complete_chain(unordered_map<Node, vector<Node> > &connections,
                                   vector<Node> &player_nodes) {
    // Get nodes from row 0
    stack<Node> stack;
    vector<Node> row_zero_nodes = get_all_nodes_of_red_from_same_row(player_nodes, 0);
    for (auto n: row_zero_nodes) {
        stack.push(n);
    }

    vector<Node> visited;
    while (!stack.empty()) {
        auto current_node = stack.top();
        stack.pop();
        visited.push_back(current_node);
        // Verify is red player had won
        if (visited.size() >= board_size) {
            if (has_completed_red_path(connections, visited)) {
                return true;
            }
        }
        // Push the next row nodes of the player
        auto row_nodes = get_all_nodes_of_red_from_same_row(player_nodes, current_node.y + 1);
        for (auto n: row_nodes) {
            stack.push(n);
        }
    }

    return false;
}

// Method that returns if blue player has won
bool is_blue_nodes_a_complete_chain(unordered_map<Node, vector<Node> > &connections,
                                    vector<Node> &player_nodes) {
    // Get nodes from column 0
    stack<Node> stack;
    vector<Node> column_zero_nodes = get_all_nodes_of_blue_from_same_column(player_nodes, 0);
    for (auto n: column_zero_nodes) {
        stack.push(n);
    }

    vector<Node> visited;
    while (!stack.empty()) {
        auto current_node = stack.top();
        stack.pop();
        visited.push_back(current_node);
        // Verify is blue player had won
        if (visited.size() >= board_size) {
            if (has_completed_blue_path(connections, visited)) {
                return true;
            }
        }
        // Push the next column nodes of the player
        auto column_nodes = get_all_nodes_of_blue_from_same_column(player_nodes, current_node.x + 1);
        for (auto n: column_nodes) {
            stack.push(n);
        }
    }

    return false;
}

// Method that returns the chosen nodes by red player in the row y
vector<Node> get_all_nodes_of_red_from_same_row(vector<Node> &nodes, int y) {
    // Make a copy of the original vector
    auto copy_nodes = nodes;

    // Sort the copy
    sort(copy_nodes.begin(), copy_nodes.end(), [](Node n1, Node n2) { return n1.x > n2.x; });

    auto predicate = [y](const Node n) {
        return n.y == y;
    };

    // Find the nodes with the condition
    auto it = find_if(begin(copy_nodes), end(copy_nodes), predicate);
    vector<Node> occurrences;
    while (it != copy_nodes.end()) {
        occurrences.push_back(*it);
        it = find_if(it + 1, copy_nodes.end(), predicate);
    }

    return occurrences;
}

// Method that returns the chosen nodes by blue player in the column x
vector<Node> get_all_nodes_of_blue_from_same_column(vector<Node> &nodes, int x) {
    // Make a copy of the original vector
    auto copy_nodes = nodes;

    // Sort the copy
    sort(copy_nodes.begin(), copy_nodes.end(), [](Node n1, Node n2) { return n1.y > n2.y; });

    auto predicate = [x](const Node n) {
        return n.x == x;
    };

    // Find the nodes with the condition
    auto it = find_if(begin(copy_nodes), end(copy_nodes), predicate);

    vector<Node> occurrences;
    while (it != copy_nodes.end()) {
        occurrences.push_back(*it);
        it = find_if(it + 1, copy_nodes.end(), predicate);
    }

    return occurrences;
}

// Method that returns if blue player has built a connected path from x = 0 to x = board_size-1
bool has_completed_blue_path(unordered_map<Node, vector<Node> > &connections, vector<Node> nodes) {
    set<int> path;
    auto init_node = nodes[0];
    path.insert(0);

    // We look for consecutive nodes for blue player starting from x = 0
    for (auto it = nodes.begin() + 1; it != nodes.end(); it++) {
        if (is_nodes_connected(connections, init_node, *it)) {
            path.insert((*it).x);
            init_node = *it;
        }
    }

    return path.size() == board_size;
}

// Method that returns if red player has built a connected path from y = 0 to y = board_size-1
bool has_completed_red_path(unordered_map<Node, vector<Node> > &connections, vector<Node> nodes) {
    set<int> path;
    auto init_node = nodes[0];
    path.insert(0);

    // We look for consecutive nodes for red player starting from y = 0
    for (auto it = nodes.begin() + 1; it != nodes.end(); it++) {
        if (is_nodes_connected(connections, init_node, *it)) {
            path.insert((*it).y);
            init_node = *it;
        }
    }

    return path.size() == board_size;
}

// Method that returns if two nodes are connected
bool is_nodes_connected(unordered_map<Node, vector<Node> > &connections, Node n1, Node n2) {
    auto neighbors = connections[n1];

    if (auto it = find_if(neighbors.begin(), neighbors.end(), [n2](Node n1) { return n1 == n2; });
        it != neighbors.end()) {
        return true;
    }
    return false;
}
