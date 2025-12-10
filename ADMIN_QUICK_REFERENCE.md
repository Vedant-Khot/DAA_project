# 🎛️ Admin Dashboard - Quick Reference

## 🚀 Access
**URL**: `frontend/admin.html`

---

## 📊 Main Features

### **Dashboard Tab**
- View real-time statistics
- See recent flights (last 10)
- Monitor activity feed
- Quick access to add flights

### **Flights Tab**
- View all flights in system
- Add new flights
- Delete existing flights
- See detailed flight information

### **Airports Tab**
- View all airports
- Add new airports
- Delete airports
- See GPS coordinates

### **Analytics Tab**
- Most popular route
- Cheapest flight price
- Most expensive flight
- Total airlines count

---

## ➕ Adding Items

### **Add Flight**:
1. Click "Add Flight" button
2. Fill form:
   - Flight ID: `FL1234`
   - Airline: Select from dropdown
   - From/To: Airport codes (e.g., `DEL`, `BOM`)
   - Date: `YYYY-MM-DD`
   - Times: `HH:MM` format
   - Duration: `2h 30m` format
   - Price: Number only
3. Submit

### **Add Airport**:
1. Go to Airports tab
2. Click "Add Airport"
3. Fill form:
   - Code: 3 letters (e.g., `DEL`)
   - City: Full name
   - Name: Airport full name
   - Lat/Lng: Decimal coordinates
4. Submit

---

## 🗑️ Deleting Items

1. Find item in table
2. Click red trash icon
3. Confirm deletion
4. Item removed

---

## 🎨 UI Elements

### **Stat Cards**:
- **Blue**: Flights data
- **Purple**: Airports data
- **Green**: Routes/Success metrics
- **Orange**: Pricing data

### **Buttons**:
- **Blue**: Primary actions (Add, Submit)
- **White**: Secondary actions (Cancel, Refresh)
- **Red**: Delete actions

### **Icons**:
- ✈️ Flights
- 📍 Airports
- 📊 Analytics
- 🔔 Activity
- ✏️ Edit
- 🗑️ Delete

---

## ⌨️ Keyboard Shortcuts

- `ESC`: Close modal
- `Enter`: Submit form (when focused)

---

## 📱 Responsive

- **Desktop**: Full layout
- **Tablet**: Adjusted grid
- **Mobile**: Stacked layout, collapsible sidebar

---

## 🔧 Configuration

### **Change Backend URL**:
```javascript
// In admin.html, line ~1100
const BASE_URL = "YOUR_BACKEND_URL";
```

### **Change Items Per Page**:
```javascript
// In loadAllFlights() function
.slice(0, 50)  // Change 50 to your desired number
```

---

## ⚠️ Important Notes

1. **No Authentication**: Anyone can access
2. **No Undo**: Deletions are permanent
3. **No Edit**: Edit feature not yet implemented
4. **Refresh Required**: Some changes need page refresh

---

## 🐛 Common Issues

| Issue | Solution |
|-------|----------|
| Data not loading | Check backend is running |
| Can't add item | Verify all fields filled |
| Delete fails | Check item exists |
| Modal stuck | Refresh page |

---

## 📞 Quick Help

**Backend not responding?**
- Check console for errors
- Verify BASE_URL is correct
- Ensure backend server is running

**Form validation errors?**
- All fields are required
- Use correct formats
- Check for duplicate IDs

**Performance slow?**
- Too many items in table
- Enable pagination
- Reduce items per page

---

## ✅ Best Practices

1. **Always confirm** before deleting
2. **Use unique IDs** for flights
3. **Verify coordinates** for airports
4. **Test in dev** before production
5. **Backup data** regularly

---

## 🎯 Quick Actions

| Action | Steps |
|--------|-------|
| View stats | Open dashboard |
| Add flight | Dashboard → Add Flight button |
| Delete flight | Find in table → Trash icon |
| View all flights | Flights tab |
| Add airport | Airports tab → Add Airport |
| See analytics | Analytics tab |
| Refresh data | Click Refresh button |

---

**Need more help?** See `ADMIN_DASHBOARD_DOCS.md` for full documentation.
