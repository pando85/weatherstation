# Weather Station

- [Weather Station](#weather-station)
    - [Hardware](#hardware)
        - [gpio](#gpio)
    - [Software](#software)
        - [Dependencies](#dependencies)
        - [Types](#types)
            - [DHT22](#dht22)
            - [NTPClient](#ntpclient)
    - [TODO](#todo)
    - [Tests](#tests)


## Hardware

In order to upload firmware, select:
    board: NodeMCU 1.0

### gpio

| BOARD | CHIP | SOFTWARE |
| D0 | IO016 | 16 |
| D1 | IO05 | 5 |
| D2 | IO04 | 4 |

## Software

### Dependencies

- DHT Adafruit library
- Adafruit Sensor common library
- Adafruit mqtt library
- Adafruit sleepydog library
- Adafruit Fona library

### Types

#### DHT22

| **Name** | **Type** | **Unit** |
| humidity | float | % |
| temperature | float | *C |
| heat index | float | *C |

#### NTPClient

| **Name** | **Type** | **Unit** |
| timestamp | usigned long | s |

## TODO

- mqtt without standard (develop homeassistant custom config)
    Could be:
      - topic: weather-station/indoor/1/pressure
      - msg: int temperature, unsigned long timestamp
- dynamic polymorphism to implement mqtt publisher

```c
String typeof(String a){
    return static const String a = "String";
}

String typeof(int a){
    return static const String a = "int";
}

String typeof(unsigned long a){
    return static const String a = "unsigned long";
}

String typeof(float a){
    return static const String = "float";
}

String typeof(char* a){
    return static const String "char*";
}


if (typeof(a) == "int")
    publish(a)
// A lo mejor todo esto es absurdo :)
```


## Tests

- Use adafruit server with AIO key