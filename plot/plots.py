import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
import os

# Читаем CSV, экспортированный из Google Benchmark
df = pd.read_csv('benchmark_results.csv')

# Функция для получения скорости MB/s для всех размеров
def get_speed(bench_name):
    data = df[df['name'].str.startswith(bench_name)].copy()
    data['idx'] = data['name'].str.split('/').str[1].astype(int)
    data = data.sort_values('idx')
    speed = data['bytes_per_second'].values / (1024*1024)
    return speed

# Размеры файлов в MB
sizes = [1, 10, 100, 500]  # изменили последний размер на 500 MB

# --- График 1: Substring Search ---
plt.figure(figsize=(8,6))
plt.plot(sizes, get_speed('NaiveSubstring'), marker='o', label='Naive')
plt.plot(sizes, get_speed('SIMDSubstring'), marker='s', label='SIMD')
plt.xlabel('Размер файла (MB)')
plt.ylabel('Скорость обработки (MB/s)')
plt.title('Сравнение скорости: Substring Search')
plt.grid(True, ls="--")
plt.legend()
plt.tight_layout()
plt.savefig('substring_search_speed.png')
plt.close()

# --- График 2: Bracket Balance ---
plt.figure(figsize=(8,6))
plt.plot(sizes, get_speed('NaiveBracket'), marker='o', label='Naive')
plt.plot(sizes, get_speed('SIMDBracket'), marker='s', label='SIMD')
plt.xlabel('Размер файла (MB)')
plt.ylabel('Скорость обработки (MB/s)')
plt.title('Сравнение скорости: Bracket Balance')
plt.grid(True, ls="--")
plt.legend()
plt.tight_layout()
plt.savefig('bracket_balance_speed.png')
plt.close()
