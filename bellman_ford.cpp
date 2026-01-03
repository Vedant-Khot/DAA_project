#include <iostream>
#include <vector>
#include <string>
#include <climits>
#include <map>
#include <algorithm>
#include <iomanip>

using namespace std;

// ==========================================
// 1. Data Structures
// ==========================================

struct Flight {
    string id;
    string from;
    string to;
    int cost;
    int depTime; // Minutes from T=0
    int arrTime; // Minutes from T=0
};

// ==========================================
// 2. Bellman-Ford Implementation
// ==========================================

class BellmanFordSolver {
public:
    // List of all flights (Edges)
    vector<Flight> allFlights;
    // Map airport codes to integers 0..N for the algorithm
    map<string, int> airportToIndex;
    map<int, string> indexToAirport;
    int n = 0; // Number of unique airports

    void addFlight(string id, string u, string v, int cost, int dep, int duration) {
        Flight f = {id, u, v, cost, dep, dep + duration};
        allFlights.push_back(f);
        
        // Register Airports if new
        if (airportToIndex.find(u) == airportToIndex.end()) {
            airportToIndex[u] = n;
            indexToAirport[n] = u;
            n++;
        }
        if (airportToIndex.find(v) == airportToIndex.end()) {
            airportToIndex[v] = n;
            indexToAirport[n] = v;
            n++;
        }
    }

    void solve(string startNode, string endNode) {
        if (airportToIndex.find(startNode) == airportToIndex.end() || 
            airportToIndex.find(endNode) == airportToIndex.end()) {
            cout << "Invalid Start or End Airport" << endl;
            return;
        }

        int src = airportToIndex[startNode];
        int dst = airportToIndex[endNode];

        // 1. Initialize Distances
        // dist[i] holds the minimum cost to reach airport i
        vector<long long> dist(n, LLONG_MAX);
        vector<int> parent(n, -1); // To reconstruct path
        vector<string> parentFlight(n, ""); // To store which flight got us there

        dist[src] = 0;

        // 2. Relax Edges (V-1 times)
        // Note: In a time-dependent graph like flights, standard Bellman-Ford checks costs.
        // It doesn't inherently handle "waiting time" unless the graph is expanded (Space-Time graph).
        // However, for this simple cost-based model where connection time is checked locally, 
        // we can try to relax. 
        // TRICKY PART: Standard BF doesn't track arrival times, so enforcing "Departure >= Arrival"
        // globally in one pass is hard without carrying state.
        // SIMPLIFICATION: We will ignore time constraints for the Pure Bellman-Ford Demo 
        // to show the "Cost" optimization, as BF is not suitable for time-dependent graphs 
        // without node expansion.
        
        for (int i = 0; i < n - 1; ++i) {
            bool changed = false;
            for (const auto& flight : allFlights) {
                int u = airportToIndex[flight.from];
                int v = airportToIndex[flight.to];

                if (dist[u] != LLONG_MAX) {
                    if (dist[u] + flight.cost < dist[v]) {
                        dist[v] = dist[u] + flight.cost;
                        parent[v] = u;
                        parentFlight[v] = flight.id + " (" + flight.from + "->" + flight.to + ")";
                        changed = true;
                    }
                }
            }
            if (!changed) break; // Optimization
        }

        // 3. Check for Negative Cycles (Not possible with flight prices > 0, but good practice)
        for (const auto& flight : allFlights) {
            int u = airportToIndex[flight.from];
            int v = airportToIndex[flight.to];
            if (dist[u] != LLONG_MAX && dist[u] + flight.cost < dist[v]) {
                cout << "Graph contains negative weight cycle!" << endl;
                return;
            }
        }

        // 4. Output Result
        if (dist[dst] == LLONG_MAX) {
            cout << "No path found from " << startNode << " to " << endNode << endl;
        } else {
            cout << "Found Cheapest Path using Bellman-Ford!" << endl;
            cout << "Total Cost: $" << dist[dst] << endl;
            
            // Reconstruct Path
            vector<string> path;
            int curr = dst;
            while (curr != -1 && curr != src) {
                path.push_back(parentFlight[curr]);
                curr = parent[curr];
            }
            reverse(path.begin(), path.end());

            cout << "Route: ";
            for (const auto& p : path) cout << "[" << p << "] ";
            cout << endl;
        }
    }
};

int main() {
    BellmanFordSolver solver;

    // Same Data as logic.cpp
    solver.addFlight("BA001", "JFK", "LHR", 400, 600, 420);
    solver.addFlight("AF022", "JFK", "CDG", 380, 660, 450);
    solver.addFlight("QR700", "JFK", "DOH", 900, 600, 780);

    solver.addFlight("BA100", "LHR", "DXB", 350, 1200, 420);
    solver.addFlight("SQ300", "LHR", "SIN", 600, 1300, 780);

    solver.addFlight("AF300", "CDG", "DXB", 340, 1300, 400);
    solver.addFlight("AF400", "CDG", "SIN", 550, 1400, 750);

    solver.addFlight("QR900", "DOH", "SYD", 700, 1600, 850);

    solver.addFlight("EK400", "DXB", "SYD", 650, 2000, 840);
    solver.addFlight("EK500", "DXB", "SIN", 300, 1800, 420);

    solver.addFlight("SQ200", "SIN", "SYD", 400, 2400, 450);
    solver.addFlight("SQ600", "SIN", "HND", 300, 2300, 360);

    string src = "JFK";
    string dst = "SYD";

    cout << "Running Bellman-Ford Algorithm (" << src << " -> " << dst << ")..." << endl;
    solver.solve(src, dst);

    return 0;
}
