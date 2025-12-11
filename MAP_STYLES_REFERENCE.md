# Map Style Options for index.html

## Current Map Style
**CartoDB Voyager** - A modern, clean map with good color contrast and visibility

## Available Map Styles

### 1. CartoDB Voyager (Current - Recommended)
```javascript
L.tileLayer('https://{s}.basemaps.cartocdn.com/rastertiles/voyager/{z}/{x}/{y}{r}.png', {
    attribution: '&copy; OpenStreetMap contributors &copy; CARTO',
    maxZoom: 19
}).addTo(map);
```
**Best for:** Modern, clean look with excellent readability

### 2. CartoDB Light (Positron)
```javascript
L.tileLayer('https://{s}.basemaps.cartocdn.com/light_all/{z}/{x}/{y}{r}.png', {
    attribution: '&copy; OpenStreetMap contributors &copy; CARTO',
    maxZoom: 19
}).addTo(map);
```
**Best for:** Minimal, light background that makes route lines stand out

### 3. CartoDB Dark Matter (Previous)
```javascript
L.tileLayer('https://{s}.basemaps.cartocdn.com/dark_all/{z}/{x}/{y}{r}.png', {
    attribution: '&copy; OpenStreetMap contributors &copy; CARTO',
    maxZoom: 19
}).addTo(map);
```
**Best for:** Dark mode, dramatic look

### 4. OpenStreetMap Standard
```javascript
L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
    attribution: '&copy; OpenStreetMap contributors',
    maxZoom: 19
}).addTo(map);
```
**Best for:** Classic, detailed map view

### 5. Stamen Terrain
```javascript
L.tileLayer('https://stamen-tiles-{s}.a.ssl.fastly.net/terrain/{z}/{x}/{y}.jpg', {
    attribution: 'Map tiles by Stamen Design, under CC BY 3.0. Data by OpenStreetMap, under ODbL.',
    maxZoom: 18,
    subdomains: 'abcd'
}).addTo(map);
```
**Best for:** Showing geographical features and terrain

### 6. Stamen Toner (High Contrast)
```javascript
L.tileLayer('https://stamen-tiles-{s}.a.ssl.fastly.net/toner/{z}/{x}/{y}.png', {
    attribution: 'Map tiles by Stamen Design, under CC BY 3.0. Data by OpenStreetMap, under ODbL.',
    maxZoom: 18,
    subdomains: 'abcd'
}).addTo(map);
```
**Best for:** Black and white, high contrast artistic look

### 7. Esri World Street Map
```javascript
L.tileLayer('https://server.arcgisonline.com/ArcGIS/rest/services/World_Street_Map/MapServer/tile/{z}/{y}/{x}', {
    attribution: 'Tiles &copy; Esri',
    maxZoom: 19
}).addTo(map);
```
**Best for:** Professional, detailed street-level view

### 8. Esri Satellite
```javascript
L.tileLayer('https://server.arcgisonline.com/ArcGIS/rest/services/World_Imagery/MapServer/tile/{z}/{y}/{x}', {
    attribution: 'Tiles &copy; Esri',
    maxZoom: 19
}).addTo(map);
```
**Best for:** Satellite imagery view

## How to Change

1. Open `index.html`
2. Find line ~1536 (the `L.tileLayer` line)
3. Replace the URL and attribution with your preferred style from above
4. Save and refresh the page

## Color Customization

You can also customize the route line colors in the `drawRouteOnMap` function (around line 1922-1930):

```javascript
// Base line (dashed background)
const baseLine = L.polyline(pathCoords, {
    color: '#3b82f6',      // Change this color
    weight: 4,
    opacity: 0.3,
    dashArray: '10, 10'
});

// Animated line (moving overlay)
const animLine = L.polyline(pathCoords, {
    color: '#8b5cf6',      // Change this color
    weight: 4,
    opacity: 0.8,
    dashArray: '10, 10',
    dashOffset: '0'
});
```

## Recommended Combinations

- **Modern & Clean:** Voyager map + Blue/Purple routes (current)
- **Professional:** Light map + Dark blue routes
- **Dramatic:** Dark map + Bright cyan/yellow routes
- **Natural:** Terrain map + Green/brown routes
