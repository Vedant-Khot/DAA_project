# Flight Booking System: Algorithmic Approach
## Presentation Outline

### Slide 1: Title Slide
**Title:** Smart Flight Booking System
**Subtitle:** Optimizing Air Travel with Advanced Graph Algorithms
**Presented by:** [Your Name/Group Name]
**Project Type:** DAA (Design and Analysis of Algorithms) Project

---

### Slide 2: Project Overview
*   **Goal:** Build a backend system to find optimal flight routes between airports.
*   **Key Challenge:** Balancing **Cost** vs. **Time** vs. **Constraints** (Dates, Layovers).
*   **Solution:** A C++ based high-performance server using Graph Algorithms.
*   **Tech Stack:** 
    *   **Language:** C++ (Crow Framework for API)
    *   **Data Storage:** JSON-based Custom DB
    *   **Architecture:** REST API

---

### Slide 3: Problem Statement
*   **Scenario:** A user wants to fly from City A to City B on a specific date.
*   **Complexity:** 
    *   Direct flights are not always available.
    *   Connecting flights (multi-hop) are cheaper but complex to calculate.
    *   Need to handle layover times and dates correctly.
*   **Objective:** Find the "Top K" best options and the absolute "Cheapest" option.

---

### Slide 4: Data Structures & Graph Modeling
*   **Nodes:** Airports (e.g., DEL, BOM, JFK).
*   **Edges:** Flights between airports.
*   **Edge Weights:** 
    *   Price (Primary weight for cost optimization).
    *   Duration (Secondary factor).
*   **Storage:** Adjacency List `Map<AirportCode, List<Flight>>`.
    *   Efficient for sparse graphs (flights are sparse compared to full mesh).

---

### Slide 5: Algorithm 1 - Dijkstra / K-Shortest Paths
*   **Used For:** Finding the top 5 distinct routes (`/api/search`).
*   **Logic:**
    *   Modified Priority Queue (Min-Heap based on Cost).
    *   Explores paths layer by layer.
    *   **Pruning:** Stop searching a node if visited > K times.
*   **Why?** Returns multiple options (Fastest, Cheapest, Balanced) for the user to choose.
*   **Complexity:** $O(K \cdot E \log V)$

---

### Slide 6: Algorithm 2 - Bellman-Ford
*   **Used For:** Finding the absolute cheapest path on a specific date (`/api/search-bellman`).
*   **Logic:**
    *   Relaxes all edges (flights) $V-1$ times.
    *   Iteratively finds cheaper paths by combining segments.
    *   Can detect negative cycles (though flight prices are always positive).
*   **Why?** A robust alternative to verify the "Cheapest" claim compared to Dijkstra.
*   **Complexity:** $O(V \cdot E)$ (Slower, but thorough).

---

### Slide 7: System Architecture
*   **Client:** Web Browser / Frontend (HTML/JS).
*   **Server:** C++ Crow (Microframework).
*   **Container:** Docker (Alpine Linux base).
*   **Database:** `JsonDB` (In-memory C++ structures, persisted to JSON).
*   **Flow:**
    1.  User searches `DEL -> BOM` on `2025-12-01`.
    2.  Server loads full graph from JSON.
    3.  Runs Algorithm (Bellman-Ford / Dijkstra).
    4.  Returns JSON response with Route, Total Price, and Segments.

---

### Slide 8: Role of Docker
*   **Purpose:** Containerization for consistent deployment.
*   **Why use it?**
    *   **Dependency Management:** Bundles C++ compiler, Crow definitions, and libraries together.
    *   **Portability:** Use `docker run` to deploy on any machine (Linux/Windows/Mac) without setting up MinGW/G++.
    *   **Isolation:** Runs the API server in a sandboxed environment.
*   **Dockerfile Strategy:**
    *   Multi-stage build (Compiles in Stage 1, Runs in lightweight Stage 2).
    *   Exposes Port 8080.


### Slide 9: Live Demo & Results
*   **Demonstration:**
    *   Route: `DEL` to `BOM`.
    *   Direct Flight: ~3252 INR.
    *   Multi-hop (if cheaper): Found via Bellman-Ford.
*   **Performance:** C++ backend ensures millisecond-level response times even with 25,000+ flights.

---

### Slide 10: Future Improvements
*   **A* Algorithm:** Use geographical distance as heuristic for faster search.
*   **Real-time Data:** Integrate live airline APIs.
*   **Caching:** Store frequent search results (e.g., DEL-BOM) to reduce computation.

---

### Slide 11: Conclusion
*   successfully implemented a dual-algorithm approach.
*   **Dijkstra** provides variety (Top K).
*   **Bellman-Ford** ensures optimality (Cheapest).
*   Modular C++ design allows easy scaling.

---
**Questions?**
