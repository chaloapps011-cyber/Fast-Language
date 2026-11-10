
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <sstream>
#include <vector>

using namespace std;

class FLEngine {
private:
    map<string, string> variables;

    vector<string> split(string text, char separator) {
        vector<string> result;
        string item;
        stringstream stream(text);

        while (getline(stream, item, separator))
            result.push_back(item);

        return result;
    }

public:
    void execute(string line, int lineNumber) {
        if (line.empty()) return;

        if (line == "end.") return;

        size_t say = line.find("_say(\"");
        size_t sayEnd = line.rfind("\")_");

        if (say != string::npos && sayEnd != string::npos) {
            cout << line.substr(say + 6, sayEnd - say - 6) << '\n';
            return;
        }

        if (line.rfind("V!_", 0) == 0) {
            size_t start = line.find('^');
            size_t finish = line.rfind('_');

            if (start == string::npos || finish <= start) {
                cerr << "Error at line " << lineNumber << '\n';
                return;
            }

            string command = line.substr(2, start - 2);
            vector<string> args = split(
                line.substr(start + 1, finish - start - 1), '^'
            );

            if (command == "create" && args.size() >= 3) {
                if (variables.count(args[0])) {
                    cerr << "Variable already exists: " << args[0] << '\n';
                    return;
                }

                variables[args[0]] = args[2];
            }
            else if (command == "set" && args.size() >= 2) {
                if (!variables.count(args[0])) {
                    cerr << "Variable not found: " << args[0] << '\n';
                    return;
                }

                variables[args[0]] = args[1];
            }
            else if (command == "show" && args.size() >= 1) {
                if (variables.count(args[0]))
                    cout << variables[args[0]] << '\n';
                else
                    cerr << "Variable not found: " << args[0] << '\n';
            }
            else if (command == "delete" && args.size() >= 1) {
                variables.erase(args[0]);
            }
            else if (args.size() >= 2) {
                if (!variables.count(args[0])) {
                    cerr << "Variable not found: " << args[0] << '\n';
                    return;
                }

                try {
                    double a = stod(variables[args[0]]);
                    double b = stod(args[1]);

                    if (command == "add") a += b;
                    else if (command == "subtract") a -= b;
                    else if (command == "multiply") a *= b;
                    else if (command == "divide") {
                        if (b == 0) {
                            cerr << "Division by zero\n";
                            return;
                        }
                        a /= b;
                    }
                    else {
                        cerr << "Unknown command: " << command << '\n';
                        return;
                    }

                    ostringstream output;
                    output << a;
                    variables[args[0]] = output.str();
                }
                catch (...) {
                    cerr << "Invalid numeric value\n";
                }
            }
        }
    }

    void run(const string& filename) {
        ifstream file(filename);

        if (!file.is_open()) {
            cerr << "Could not open: " << filename << '\n';
            return;
        }

        string line;
        int lineNumber = 0;
        bool ended = false;

        cout << "FL Engine 0.4\n";
        cout << "----------------\n";

        while (getline(file, line)) {
            lineNumber++;

            if (line == "end.") {
                ended = true;
                break;
            }

            execute(line, lineNumber);
        }

        if (!ended)
            cerr << "Error: end. not found\n";
        else
            cout << "\nExecution finished.\n";
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Usage: fl_engine arquivo.fl\n";
        return 1;
    }

    FLEngine engine;
    engine.run(argv[1]);

    return 0;
}
