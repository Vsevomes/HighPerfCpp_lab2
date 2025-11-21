# HighPerfCpp_lab2

## Project structure

```
project/
│
├── CMakeLists.txt
├── README.md
├── main.cpp
├── src/
│   ├── substring_search.cpp             
│   └── bracket_balance.cpp
├── include/
│   ├── substring_search.h             
│   └── bracket_balance.h     
│
└── plot/
    ├── plots.py          
    └── requirements.txt   
```

## Build project

```bash
mkdir build
cd build
cmake ..
-j$(nproc)
```

## Execute project

```bash
./simd_bench --benchmark_format=console --benchmark_out_format=csv --benchmark_out=../plot/benchmark_results.csv
```

## Create plots

```bash
cd ../plot
python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt
python3 plots.py
deactivate
```

Results will appear in plot directory