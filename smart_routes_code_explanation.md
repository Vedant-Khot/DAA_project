# Smart Routes Algorithm - Code Explanation
## Line-by-Line Technical Breakdown

---

## 📁 File Structure Overview

The algorithm spans across 3 files:

| File | Purpose |
|------|---------|
| `jsondb.h` | Declares the `Edge` struct and `JsonDB` class |
| `Models.h` | Defines `Airport` and `Flight` data models |
| `jsondb.cpp` | Contains the actual algorithm implementation |

---

## 🧱 Data Structures Used

### 1. Edge Structure (from `jsondb.h`)

```cpp
struct Edge {
    std::string destination;    // Target airport code (e.g., "BOM")
    int weight_minutes;         // Flight duration in minutes
    std::string flight_id;      // Unique flight ID (e.g., "FL1001")
    std::string date;           // Flight date (e.g., "2025-12-10")
    std::string dep_time;       // Departure time (e.g., "14:30")
    std::string arr_time;       // Arrival time (e.g., "16:45")
    int price;                  // Price in INR
    std::string airline;        // Airline name (e.g., "IndiGo")
};
```

**Purpose:** Represents a single flight as a graph edge. Each edge connects one airport to another.

---

### 2. PathState Structure (from `jsondb.cpp`)

```cpp
struct PathState {
    int total_minutes;           // Total travel time so far
    string current_node;         // Current airport code
    vector<Edge> history;        // All flights taken to reach here

    // Comparison operator for priority queue (min-heap)
    bool operator>(const PathState& other) const {
        return total_minutes > other.total_minutes;
    }
};
```

**Purpose:** Tracks the current state of exploration. Each state represents "I'm at airport X, having taken these flights, with total time Y."

**Why `operator>`?** The priority queue uses this to sort states. By returning `true` when `total_minutes` is **greater**, we create a **min-heap** (smallest time first).

---

### 3. Adjacency List (Graph Storage)

```cpp
// In jsondb.h (class member)
std::unordered_map<std::string, std::vector<Edge>> adj_list;
```

**Structure Visualization:**
```
adj_list = {
    "DEL": [Edge→BOM, Edge→BLR, Edge→MAA, ...],
    "BOM": [Edge→DEL, Edge→BLR, Edge→HYD, ...],
    "BLR": [Edge→DEL, Edge→BOM, Edge→MAA, ...],
    ...
}
```

**Purpose:** Quick O(1) lookup to find all flights departing from any airport.

---

## 🔨 Building the Graph

### `build_graph()` Function

```cpp
void JsonDB::build_graph() {
    adj_list.clear();  // Reset the graph
    
    if (!data.contains("flights")) return;  // Safety check

    for (const auto& f : data["flights"]) {
        Edge e;
        e.destination = f["to_code"];           // Where the flight goes
        e.flight_id = f["id"];                  // Flight identifier
        e.date = f["date"];                     // Travel date
        e.dep_time = f["departure"];            // Departure time
        e.arr_time = f["arrival"];              // Arrival time
        e.price = f["price"];                   // Ticket price
        e.airline = f["airline"];               // Airline name
        e.weight_minutes = parse_duration_string(f["duration"]);  // Convert "2h 30m" → 150

        adj_list[f["from_code"]].push_back(e);  // Add edge to source airport's list
    }
}
```

**What Happens:**
1. Clears any existing graph data
2. Loops through every flight in the JSON database
3. Creates an `Edge` object for each flight
4. Adds the edge to the source airport's adjacency list

**Called When:** Constructor (`JsonDB()`) and after any `save()` operation.

---

## ⏱️ Duration Parser

### `parse_duration_string()` Function

```cpp
int JsonDB::parse_duration_string(const string& dur) {
    try {
        size_t h_pos = dur.find('h');           // Find 'h' character
        size_t m_pos = dur.find('m');           // Find 'm' character
        if (h_pos == string::npos) return 0;    // No hours found
        
        int hours = stoi(dur.substr(0, h_pos)); // Extract hours (before 'h')
        int mins = 0;
        
        size_t space_pos = dur.find(' ');
        if (space_pos != string::npos && m_pos != string::npos) {
            // Extract minutes (between space and 'm')
            mins = stoi(dur.substr(space_pos + 1, m_pos - space_pos - 1));
        }
        return (hours * 60) + mins;  // Convert to total minutes
    } catch (...) {
        return 60;  // Default fallback: 1 hour
    }
}
```

**Example Conversions:**
| Input | Output |
|-------|--------|
| `"2h 30m"` | 150 |
| `"1h 45m"` | 105 |
| `"3h 00m"` | 180 |

---

## 🧠 The Main Algorithm

### `find_smart_routes()` Function - Complete Breakdown

```cpp
json JsonDB::find_smart_routes(const string& src, const string& dst, 
                                const string& req_date, int k) {
```

**Parameters:**
- `src` - Source airport code (e.g., "DEL")
- `dst` - Destination airport code (e.g., "MAA")
- `req_date` - Travel date (e.g., "2025-12-10")
- `k` - Number of routes to find (default: 5)

---

### Step 1: Thread Safety

```cpp
    lock_guard<mutex> lock(db_mutex);
```

**Purpose:** Prevents race conditions when multiple requests access the database simultaneously. The lock is automatically released when the function exits.

---

### Step 2: Initialize Data Structures

```cpp
    json results = json::array();  // Final output: array of routes
    
    // Priority queue: min-heap ordered by total_minutes
    priority_queue<PathState, vector<PathState>, greater<PathState>> pq;
    
    // Start exploration from source with 0 time and empty history
    pq.push({0, src, {}});

    // Track how many times each airport has been visited
    unordered_map<string, int> visits;
```

**Why Min-Heap?** 
- `greater<PathState>` makes it a min-heap
- Popping always gives us the state with the **smallest** total time
- This ensures we find shortest routes first

---

### Step 3: Main Exploration Loop

```cpp
    while (!pq.empty() && results.size() < k) {
        PathState top = pq.top();   // Get best (shortest) state
        pq.pop();                    // Remove it from queue

        string u = top.current_node; // Current airport
```

**Loop Continues Until:**
- Queue is empty (no more paths to explore), OR
- We've found K routes

---

### Step 4: Destination Check

```cpp
        if (u == dst) {
            // We've reached the destination! Build the route object
            json route;
            route["total_time"] = top.total_minutes;
            
            // Format duration as "Xh Ym"
            int h = top.total_minutes / 60;
            int m = top.total_minutes % 60;
            route["duration_fmt"] = to_string(h) + "h " + to_string(m) + "m";
            
            // Number of stops = number of flights - 1
            route["stops"] = (int)top.history.size() - 1;
```

**What's Happening:**
- If current position equals destination, we found a valid route
- Calculate formatted duration from total minutes
- Stops = total flights minus 1 (direct flight = 0 stops)

---

### Step 5: Build Route Segments

```cpp
            json segments = json::array();
            string current_from = src;  // Start tracking from source

            for(const auto& h : top.history) {
                segments.push_back({
                    {"airline", h.airline},
                    {"flight_id", h.flight_id},
                    {"from", current_from},      // Previous airport
                    {"to", h.destination},       // This flight's destination
                    {"dep", h.dep_time},
                    {"arr", h.arr_time},
                    {"price", h.price},
                    {"date", h.date}
                });
                current_from = h.destination;  // Update for next segment
            }
            
            route["segments"] = segments;
```

**Example Output:**
```json
{
  "segments": [
    {"from": "DEL", "to": "BOM", "airline": "IndiGo", ...},
    {"from": "BOM", "to": "MAA", "airline": "Vistara", ...}
  ]
}
```

---

### Step 6: Calculate Total Price

```cpp
            int total_price = 0;
            for(const auto& s : segments) 
                total_price += (int)s["price"];
            route["total_price"] = total_price;

            results.push_back(route);  // Add to final results
            continue;  // Go to next iteration
        }
```

**Purpose:** Sum up prices of all flight segments for total route cost.

---

### Step 7: Visit Limiting (Optimization)

```cpp
        if (visits[u] >= k) continue;  // Skip if visited too many times
        visits[u]++;                    // Increment visit counter
```

**Why This Matters:**
- Without this, the algorithm could explore the same airport infinitely
- Limits each airport to K visits maximum
- Allows finding K different paths through the same intermediate airports

---

### Step 8: Explore Outgoing Flights

```cpp
        if (adj_list.find(u) != adj_list.end()) {
            for (const auto& edge : adj_list[u]) {
```

**Purpose:** Loop through all flights departing from current airport.

---

### Step 9: Filter - Date Check

```cpp
                if (edge.date != req_date) continue;
```

**Purpose:** Only consider flights on the requested travel date.

---

### Step 10: Filter - Cycle Detection

```cpp
                bool cycle = false;
                for(const auto& prev : top.history) {
                    // Check if destination is source OR already visited
                    if (edge.destination == src || 
                        prev.destination == edge.destination) 
                        cycle = true;
                }
                if (cycle) continue;
```

**Purpose:** Prevent infinite loops like DEL→BOM→DEL→BOM...

**Logic:**
- Can't go back to the source airport
- Can't revisit any airport already in our path

---

### Step 11: Filter - Time Feasibility

```cpp
                if (!top.history.empty()) {
                    string prev_arr = top.history.back().arr_time;
                    if (edge.dep_time < prev_arr) continue;
                }
```

**Purpose:** Ensure connecting flight departs AFTER we arrive.

**Example:**
- Previous flight arrives at 14:00
- Next flight departs at 13:30
- ❌ Impossible! Skip this flight.

---

### Step 12: Add Valid Path to Queue

```cpp
                vector<Edge> new_history = top.history;  // Copy current history
                new_history.push_back(edge);              // Add new flight
                
                int layover = top.history.empty() ? 0 : 60;  // 60 min buffer for connections

                pq.push({
                    top.total_minutes + edge.weight_minutes + layover,  // New total time
                    edge.destination,                                     // New position
                    new_history                                           // Updated path
                });
            }
        }
    }

    return results;  // Return all found routes
}
```

**What's Happening:**
1. Create a copy of the current path history
2. Add the new flight to the history
3. Add 60-minute layover for connecting flights (not for first flight)
4. Push the new state to the priority queue

---

## 📊 Complete Code Flow Diagram

```
find_smart_routes("DEL", "MAA", "2025-12-10", 3)
│
├─► Lock mutex for thread safety
│
├─► Initialize:
│   ├─ results = []
│   ├─ pq = [{0, "DEL", []}]
│   └─ visits = {}
│
└─► WHILE pq not empty AND results.size() < 3:
    │
    ├─► Pop top state (lowest time)
    │
    ├─► IF at destination "MAA":
    │   ├─ Build route JSON
    │   ├─ Add to results
    │   └─ continue
    │
    ├─► IF visits["current"] >= 3: continue
    │
    ├─► visits["current"]++
    │
    └─► FOR each flight from current airport:
        │
        ├─► IF wrong date: continue
        ├─► IF creates cycle: continue
        ├─► IF timing impossible: continue
        │
        └─► Push new state to pq:
            {time + flight_duration + layover, 
             destination, 
             history + this_flight}
```

---

## 🎯 Return Value Structure

```json
[
  {
    "total_time": 120,
    "duration_fmt": "2h 0m",
    "stops": 0,
    "total_price": 5500,
    "segments": [
      {
        "airline": "IndiGo",
        "flight_id": "FL1234",
        "from": "DEL",
        "to": "MAA",
        "dep": "09:00",
        "arr": "11:00",
        "price": 5500,
        "date": "2025-12-10"
      }
    ]
  },
  {
    "total_time": 210,
    "duration_fmt": "3h 30m",
    "stops": 1,
    "total_price": 4200,
    "segments": [
      {"from": "DEL", "to": "BLR", ...},
      {"from": "BLR", "to": "MAA", ...}
    ]
  }
]
```

---

## ⚙️ Complexity Analysis

| Aspect | Complexity |
|--------|------------|
| Time | O(K × V × (E + V log V)) |
| Space | O(K × V) for path storage |

Where:
- K = number of routes to find
- V = number of airports (vertices)
- E = number of flights (edges)

---

## 🔗 Function Call Chain

```
User Request
     │
     ▼
find_smart_routes()
     │
     ├──► Uses adj_list (built by build_graph())
     │
     ├──► build_graph()
     │         │
     │         └──► parse_duration_string()
     │
     └──► Returns JSON array of routes
```

---

## 📝 Key Takeaways

1. **Priority Queue** ensures shortest paths are found first
2. **Visit limiting** prevents exponential exploration
3. **Cycle detection** avoids infinite loops
4. **Time validation** ensures realistic connections
5. **History tracking** enables complete path reconstruction
6. **Thread safety** via mutex for concurrent access
