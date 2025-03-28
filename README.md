# Smart light system with ESP32 WROOM and LoRa
## Build Step
The programe has 2 modes include GATEWAY and NODE. This modo will be choosen at the compile time.
### 1. Gateway mode
Open `platformio.ini` and find field `build_flags`. Add the command `-DUSING_GATEWAY_MODE` like below
```ini
build_flags = 
  -DCORE_DEBUG_LEVEL=0
  -DUSING_GATEWAY_MODE
```

### 2. Node mode
Open `platformio.ini` and find field `build_flags`. Add the command `-DUSING_NODE_MODE` like below
```ini
build_flags = 
  -DCORE_DEBUG_LEVEL=0
  -DUSING_NODE_MODE
```
