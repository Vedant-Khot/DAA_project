// const BASE_URL = "http://localhost:18080";
const BASE_URL = "https://daa-project-1-7w2x.onrender.com";
let allFlights = [];
let allAirports = [];
let currentFlightPage = 1;
const flightsPerPage = 10;

// Initialize
document.addEventListener('DOMContentLoaded', () => {
    loadSidebar();

    // Determine which page specific loader to run based on URL
    const path = window.location.pathname;
    if (path.includes('admin-flights.html')) loadAllFlights();
    else if (path.includes('admin-airports.html')) loadAllAirports();
    else if (path.includes('admin-analytics.html')) loadAnalytics();
    else if (path.includes('admin-users.html')) loadUsers();
    else if (path.includes('admin-bookings.html')) loadAllBookings();
    else if (path.endsWith('admin.html')) loadDashboardData();
});

// Sidebar Loader
function loadSidebar() {
    const sidebarHTML = `
    <div class="sidebar">
        <div class="sidebar-header">
            <div class="sidebar-logo">
                <i class="fas fa-plane-departure"></i>
                <div>
                    <h1>AOPPs Admin</h1>
                    <p>Management Portal</p>
                </div>
            </div>
        </div>
        <ul class="sidebar-menu">
            <li class="menu-item">
                <a href="admin.html" id="link-dashboard">
                    <i class="fas fa-chart-line"></i>
                    <span>Dashboard</span>
                </a>
            </li>
            <li class="menu-item">
                <a href="admin-flights.html" id="link-flights">
                    <i class="fas fa-plane"></i>
                    <span>Flights</span>
                </a>
            </li>
            <li class="menu-item">
                <a href="admin-airports.html" id="link-airports">
                    <i class="fas fa-map-marker-alt"></i>
                    <span>Airports</span>
                </a>
            </li>
            <li class="menu-item">
                <a href="admin-analytics.html" id="link-analytics">
                    <i class="fas fa-chart-pie"></i>
                    <span>Analytics</span>
                </a>
            </li>
            <li class="menu-item">
                <a href="admin-bookings.html" id="link-bookings">
                    <i class="fas fa-ticket-alt"></i>
                    <span>Bookings</span>
                </a>
            </li>
            <li class="menu-item">
                <a href="admin-users.html" id="link-users">
                    <i class="fas fa-users"></i>
                    <span>Users</span>
                </a>
            </li>
            <li class="menu-item">
                <a href="index.html">
                    <i class="fas fa-home"></i>
                    <span>Back to Site</span>
                </a>
            </li>
        </ul>
    </div>
    `;

    document.getElementById('sidebar-container').innerHTML = sidebarHTML;

    // Set Active Link
    const path = window.location.pathname;
    const links = {
        'admin.html': 'link-dashboard',
        'admin-flights.html': 'link-flights',
        'admin-airports.html': 'link-airports',
        'admin-analytics.html': 'link-analytics',
        'admin-users.html': 'link-users',
        'admin-bookings.html': 'link-bookings'
    };

    for (const [page, id] of Object.entries(links)) {
        if (path.endsWith(page)) {
            document.getElementById(id).classList.add('active');
            break;
        }
    }
}

// Modal Functions
function openModal(modalId) {
    document.getElementById(modalId).classList.add('active');
}

function closeModal(modalId) {
    document.getElementById(modalId).classList.remove('active');
}

// Global Refresh Helper
function refreshData() {
    window.location.reload();
}

// Load Dashboard Data
async function loadDashboardData() {
    try {
        const statsRes = await fetch(`${BASE_URL}/api/admin/stats`);
        const stats = await statsRes.json();
        const [flightsRes, airportsRes, bookingsRes] = await Promise.all([
            fetch(`${BASE_URL}/api/flights?limit=10`),
            fetch(`${BASE_URL}/api/airports`),
            fetch(`${BASE_URL}/api/bookings`)
        ]);

        const flightsData = await flightsRes.json();
        allFlights = flightsData.flights || []; // Handle paginated structure
        allAirports = await airportsRes.json();
        const allBookings = await bookingsRes.json() || [];

        updateStats(stats);
        renderRecentFlights(allFlights);
        renderActivityFeed(allBookings);
    } catch (error) {
        console.error('Error loading dashboard:', error);
    }
}

function updateStats(stats) {
    if (!document.getElementById('totalFlights')) return;
    document.getElementById('totalFlights').textContent = (stats.total_flights || 0).toLocaleString();
    document.getElementById('totalAirports').textContent = stats.total_airports || 0;
    document.getElementById('activeRoutes').textContent = stats.popular_route === "N/A" ? 0 : (stats.total_flights / 2).toFixed(0);
    document.getElementById('avgPrice').textContent = stats.cheapest_price ? '₹' + stats.cheapest_price.toLocaleString() : '₹0';
    document.getElementById('totalUsers').textContent = stats.total_users || 0;
    document.getElementById('totalRevenue').textContent = '₹' + (stats.total_revenue || 0).toLocaleString();
}

function renderRecentFlights(flights = []) {
    const container = document.getElementById('recentFlightsTable');
    if (!container) return;
    const html = `
        <table class="data-table">
            <thead>
                <tr>
                    <th>Flight ID</th>
                    <th>Route</th>
                    <th>Airline</th>
                    <th>Date</th>
                    <th>Price</th>
                    <th>Actions</th>
                </tr>
            </thead>
            <tbody>
                ${flights.map(flight => `
                    <tr>
                        <td><strong>${flight.id}</strong></td>
                        <td>${flight.from_code} → ${flight.to_code}</td>
                        <td>${flight.airline}</td>
                        <td>${flight.date}</td>
                        <td><strong>₹${flight.price.toLocaleString()}</strong></td>
                        <td>
                            <div class="action-buttons">
                                <button class="btn-icon edit" onclick="editFlight('${flight.id}')">
                                    <i class="fas fa-edit"></i>
                                </button>
                                <button class="btn-icon delete" onclick="deleteFlight('${flight.id}')">
                                    <i class="fas fa-trash"></i>
                                </button>
                            </div>
                        </td>
                    </tr>
                `).join('')}
            </tbody>
        </table>
    `;
    container.innerHTML = html;
}

function renderActivityFeed(bookings = []) {
    const container = document.getElementById('activityFeed');
    if (!container) return;

    // Sort and take 5
    const latest = [...bookings].sort((a, b) => new Date(b.booking_date) - new Date(a.booking_date)).slice(0, 5);

    if (latest.length === 0) {
        container.innerHTML = `<div class="loading"><p>No recent activity</p></div>`;
        return;
    }

    const html = latest.map(b => `
        <div class="activity-item">
            <div class="activity-icon ${b.status === 'confirmed' ? 'green' : 'red'}">
                <i class="fas fa-${b.status === 'confirmed' ? 'check-circle' : 'times-circle'}"></i>
            </div>
            <div class="activity-content">
                <h4>Booking ${b.status}</h4>
                <p><strong>${b.passenger_name}</strong> booked ${b.from_code} → ${b.to_code}</p>
                <div class="activity-time">${b.booking_date} (ID: ${b.booking_id})</div>
            </div>
        </div>
    `).join('');
    container.innerHTML = html;
}

// FLIGHT FUNCTIONS
async function loadAllFlights(page = 1, searchQuery = "") {
    const container = document.getElementById('allFlightsTable');
    if (!container) return;

    currentFlightPage = page;
    const query = searchQuery || document.getElementById('flightSearchInput')?.value || "";

    try {
        const response = await fetch(`${BASE_URL}/api/flights?page=${page}&limit=${flightsPerPage}&search=${encodeURIComponent(query)}`);
        const data = await response.json();

        let totalPages = 1;
        if (Array.isArray(data)) {
            // Fallback for old API / not restarted server
            allFlights = data;
        } else {
            // New paginated API
            allFlights = data.flights || [];
            totalPages = data.totalPages || 1;
        }

        if (allFlights.length === 0) {
            container.innerHTML = `<div class="loading"><p>No flights found matching "${query}"</p></div>`;
            return;
        }

        const html = `
            <table class="data-table">
                <thead>
                    <tr>
                        <th>Flight ID</th>
                        <th>Airline</th>
                        <th>Route</th>
                        <th>Date</th>
                        <th>Departure</th>
                        <th>Price</th>
                        <th>Actions</th>
                    </tr>
                </thead>
                <tbody>
                    ${allFlights.map(flight => `
                        <tr>
                            <td><strong>${flight.id}</strong></td>
                            <td>${flight.airline}</td>
                            <td>${flight.from_code} → ${flight.to_code}</td>
                            <td>${flight.date}</td>
                            <td>${flight.departure}</td>
                            <td><strong>₹${flight.price.toLocaleString()}</strong></td>
                            <td>
                                <div class="action-buttons">
                                    <button class="btn-icon edit" onclick="editFlight('${flight.id}')"><i class="fas fa-edit"></i></button>
                                    <button class="btn-icon delete" onclick="deleteFlight('${flight.id}')"><i class="fas fa-trash"></i></button>
                                </div>
                            </td>
                        </tr>
                    `).join('')}
                </tbody>
            </table>
            
            <div class="pagination-controls">
                <button class="btn btn-secondary" onclick="loadAllFlights(${page - 1})" ${page <= 1 ? 'disabled' : ''}>
                    <i class="fas fa-chevron-left"></i> Prev
                </button>
                <span>Page ${page} of ${totalPages}</span>
                <button class="btn btn-secondary" onclick="loadAllFlights(${page + 1})" ${page >= totalPages ? 'disabled' : ''}>
                    Next <i class="fas fa-chevron-right"></i>
                </button>
            </div>
        `;
        container.innerHTML = html;

    } catch (e) {
        console.error(e);
        container.innerHTML = '<div class="loading"><p>Error loading flights</p></div>';
    }
}

function searchFlights() {
    loadAllFlights(1);
}

async function addFlight(event) {
    event.preventDefault();
    const formData = new FormData(event.target);
    const flight = Object.fromEntries(formData.entries());

    // Convert price to number
    flight.price = parseInt(flight.price);

    try {
        const response = await fetch(`${BASE_URL}/admin/flight/add`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(flight)
        });

        if (response.ok) {
            closeModal('addFlightModal');
            alert('Flight added successfully');
            window.location.reload();
        } else {
            const errorMsg = await response.text();
            alert(`Failed to add flight: ${errorMsg}`);
        }
    } catch (error) {
        console.error('Error:', error);
        alert('Error adding flight');
    }
}

async function deleteFlight(id) {
    if (confirm('Are you sure you want to delete this flight?')) {
        try {
            const response = await fetch(`${BASE_URL}/admin/flight/delete`, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ id: id })
            });

            if (response.ok) {
                window.location.reload();
            } else {
                alert('Failed to delete flight');
            }
        } catch (error) {
            console.error('Error:', error);
            alert('Error deleting flight');
        }
    }
}

async function updateFlight(event) {
    event.preventDefault();
    const formData = new FormData(event.target);
    const flight = Object.fromEntries(formData.entries());
    const originalId = flight.originalId;
    delete flight.originalId;

    // Convert price to number
    flight.price = parseInt(flight.price);

    try {
        const response = await fetch(`${BASE_URL}/admin/flight/update?id=${originalId}`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(flight)
        });

        if (response.ok) {
            closeModal('editFlight');
            alert('Flight updated successfully');
            window.location.reload();
        } else {
            alert('Failed to update flight');
        }
    } catch (error) {
        console.error('Error:', error);
        alert('Error updating flight');
    }
}

// AIRPORT FUNCTIONS
async function loadAllAirports() {
    const container = document.getElementById('airportsTable');
    if (!container) return;

    try {
        const response = await fetch(`${BASE_URL}/api/airports`);
        allAirports = await response.json();
    } catch (e) { console.error(e); }

    const html = `
        <table class="data-table">
            <thead>
                <tr>
                    <th>Code</th>
                    <th>City</th>
                    <th>Airport Name</th>
                    <th>Actions</th>
                </tr>
            </thead>
            <tbody>
                ${allAirports.map(airport => `
                    <tr>
                        <td><strong>${airport.code}</strong></td>
                        <td>${airport.city}</td>
                        <td>${airport.name}</td>
                        <td>
                            <div class="action-buttons">
                                <button class="btn-icon edit" onclick="editAirport('${airport.code}')"><i class="fas fa-edit"></i></button>
                                <button class="btn-icon delete" onclick="deleteAirport('${airport.code}')"><i class="fas fa-trash"></i></button>
                            </div>
                        </td>
                    </tr>
                `).join('')}
            </tbody>
        </table>
    `;
    container.innerHTML = html;
}

async function addAirport(event) {
    event.preventDefault();
    const formData = new FormData(event.target);
    const airport = Object.fromEntries(formData.entries());

    // Convert lat/lng to numbers
    airport.lat = parseFloat(airport.lat);
    airport.lng = parseFloat(airport.lng);

    try {
        const response = await fetch(`${BASE_URL}/admin/airport/add`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(airport)
        });

        if (response.ok) {
            closeModal('addAirportModal');
            alert('Airport added successfully');
            window.location.reload();
        } else {
            alert('Failed to add airport');
        }
    } catch (error) {
        console.error('Error:', error);
        alert('Error adding airport');
    }
}

async function deleteAirport(code) {
    if (confirm('Are you sure you want to delete this airport?')) {
        try {
            const response = await fetch(`${BASE_URL}/admin/airport/delete`, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ code: code })
            });

            if (response.ok) {
                window.location.reload();
            } else {
                alert('Failed to delete airport');
            }
        } catch (error) {
            console.error('Error:', error);
            alert('Error deleting airport');
        }
    }
}

async function updateAirport(event) {
    event.preventDefault();
    const formData = new FormData(event.target);
    const airport = Object.fromEntries(formData.entries());
    const originalCode = document.getElementById('editAirportOriginalCode')?.value; // Get hidden originalCode

    // Convert lat/lng
    if (airport.lat) airport.lat = parseFloat(airport.lat);
    if (airport.lng) airport.lng = parseFloat(airport.lng);

    try {
        const response = await fetch(`${BASE_URL}/admin/airport/update?code=${originalCode}`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(airport)
        });

        if (response.ok) {
            closeModal('editAirport'); // Assuming modal ID is editAirport
            alert('Airport updated successfully');
            window.location.reload();
        } else {
            alert('Failed to update airport');
        }
    } catch (error) {
        console.error('Error:', error);
        alert('Error updating airport');
    }
}

// BOOKING FUNCTIONS
async function loadAllBookings() {
    const container = document.getElementById('allBookingsTable');
    if (!container) return;

    try {
        const response = await fetch(`${BASE_URL}/api/bookings`);
        const bookings = await response.json();

        if (!bookings || bookings.length === 0) {
            container.innerHTML = `<div class="loading"><i class="fas fa-ticket-alt"></i><p>No bookings found</p></div>`;
            return;
        }

        bookings.sort((a, b) => new Date(b.booking_date) - new Date(a.booking_date));

        const html = `
            <table class="data-table">
                <thead>
                    <tr>
                        <th>ID</th>
                        <th>Passenger</th>
                        <th>Route</th>
                        <th>Date</th>
                        <th>Status</th>
                        <th>Actions</th>
                    </tr>
                </thead>
                <tbody>
                    ${bookings.map(b => `
                        <tr>
                            <td><code style="font-size:11px;">${b.booking_id}</code></td>
                            <td>${b.passenger_name}</td>
                            <td>${b.from_code} → ${b.to_code}</td>
                            <td>${b.date}</td>
                            <td><span class="badge ${b.status === 'confirmed' ? 'success' : 'danger'}">${b.status}</span></td>
                            <td>
                                <div class="action-buttons">
                                    <a href="ticket.html?booking_id=${b.booking_id}" target="_blank" class="btn-icon edit"><i class="fas fa-ticket-alt"></i></a>
                                    <button class="btn-icon delete" onclick="cancelBooking('${b.booking_id}')" ${b.status === 'cancelled' ? 'disabled' : ''}><i class="fas fa-times"></i></button>
                                </div>
                            </td>
                        </tr>
                    `).join('')}
                </tbody>
            </table>
        `;
        container.innerHTML = html;
    } catch (e) { container.innerHTML = '<p>Error loading bookings</p>'; }
}

async function cancelBooking(id) {
    if (!confirm(`Cancel booking ${id}?`)) return;
    try {
        // Mock backend call
        alert('Booking cancellation simulation');
        loadAllBookings();
    } catch (e) { console.error(e); }
}

// ANALYTICS FUNCTIONS
async function loadAnalytics() {
    try {
        const statsRes = await fetch(`${BASE_URL}/api/admin/stats`);
        const s = await statsRes.json();
        const flightsRes = await fetch(`${BASE_URL}/api/flights?limit=2000`); // Get more for stats
        const flightData = await flightsRes.json();
        const flights = flightData.flights || [];

        // Update UI
        if (document.getElementById('cheapestPrice'))
            document.getElementById('cheapestPrice').textContent = '₹' + (s.cheapest_price || 0).toLocaleString();

        if (document.getElementById('expensivePrice'))
            document.getElementById('expensivePrice').textContent = '₹' + (s.expensive_price || 0).toLocaleString();

        if (document.getElementById('popularRoute'))
            document.getElementById('popularRoute').textContent = s.popular_route || 'N/A';

        const trendingEl = document.getElementById('popularRoute')?.nextElementSibling;
        if (trendingEl) {
            trendingEl.innerHTML = `<i class="fas fa-users"></i> ${s.popular_route_count || 0} Passengers`;
        }

        const airlines = new Set(flights.map(f => f.airline));
        if (document.getElementById('airlinesCount'))
            document.getElementById('airlinesCount').textContent = airlines.size || 0;

    } catch (e) { console.error(e); }
}

// USERS FUNCTIONS
async function loadUsers() {
    const container = document.getElementById('usersTable');
    if (!container) return;

    try {
        const response = await fetch(`${BASE_URL}/api/users`);
        const users = await response.json();

        if (!users || users.length === 0) {
            container.innerHTML = `<div class="loading"><i class="fas fa-users-slash"></i><p>No registered users found</p></div>`;
            return;
        }

        const html = `
            <table class="data-table">
                <thead>
                    <tr>
                        <th>User ID</th>
                        <th>Name</th>
                        <th>Email</th>
                        <th>Joined Date</th>
                        <th>Actions</th>
                    </tr>
                </thead>
                <tbody>
                    ${users.map(u => `
                        <tr>
                            <td><code style="font-size:11px;">${u.id}</code></td>
                            <td>${u.name}</td>
                            <td>${u.email}</td>
                            <td>${u.created_at || 'N/A'}</td>
                            <td>
                                <div class="action-buttons">
                                    <button class="btn-icon delete" onclick="alert('Delete user simulation')"><i class="fas fa-trash"></i></button>
                                </div>
                            </td>
                        </tr>
                    `).join('')}
                </tbody>
            </table>
        `;
        container.innerHTML = html;

    } catch (e) {
        console.error(e);
        container.innerHTML = '<div class="loading"><p>Error loading users</p></div>';
    }
}
