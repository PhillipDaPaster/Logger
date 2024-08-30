# ImGui Logger

## Usage

### Draw logs on the screen at position (50, 50)
```cpp
D::log.draw(ImVec2(50, 50), true, ImColor(255, 255, 0, 255));
```

### Add a log entry
```cpp
add_log( <(Const Char PRE-TEXT)>, <(Const Char TEXT)>, <( Int Interval )>, <( Bool RunOnce )> );

D::log.add_log("[INFO] ", "This is a log entry.");
```

### Example of Log Entry
```cpp
D::log.add_log("[INFO] ", "This is a log entry.");
```

### Toggle auto-scroll on and off
```cpp
D::log.setautoscroll(true);
D::log.setautoscroll(false);
```

### Clear all log entries
```cpp
D::log.clear();
```
