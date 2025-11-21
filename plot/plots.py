import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# --- Чтение CSV с пропуском заголовков от Google Benchmark ---
with open("benchmark_results.csv") as f:
    lines = f.readlines()

for header_index, line in enumerate(lines):
    if line.startswith("name,"):
        break

df = pd.read_csv("benchmark_results.csv", skiprows=header_index)

# --- Обработка данных ---
df["method"] = df["name"].apply(lambda x: x.split("/")[0])
df["idx"] = df["name"].apply(lambda x: int(x.split("/")[1]))

sizes_bytes = {
    0: 1_000,
    1: 10_000,
    2: 100_000,
    3: 1_000_000,
    4: 10_000_000,
    5: 100_000_000
}

df["size_bytes"] = df["idx"].map(sizes_bytes)

def human_size(b):
    if b < 1_000_000:
        return f"{b//1000} KB"
    return f"{b//1_000_000} MB"

df["size_label"] = df["size_bytes"].apply(human_size)

substring = df[df["method"].str.contains("Substring")]
brackets = df[df["method"].str.contains("Bracket")]

# --- TIME GRAPH (линейные) ---
def plot_time(data, title, filename):
    plt.figure(figsize=(10, 6))

    for method in data["method"].unique():
        sub = data[data["method"] == method]
        plt.plot(sub["size_label"], sub["real_time"], marker='o', label=method)

    plt.xlabel("Размер данных")
    plt.ylabel("Время (мс)")
    plt.title(title)
    plt.grid(True)
    plt.legend()
    plt.tight_layout()
    plt.savefig(filename)
    plt.close()

plot_time(substring, "Substring — время выполнения", "substring_time.png")
plot_time(brackets, "Bracket — время выполнения", "bracket_time.png")


# --- THROUGHPUT BAR CHART ---
def plot_throughput_bar(data, title, filename):
    plt.figure(figsize=(12, 6))

    methods = sorted(data["method"].unique())
    sizes = data["size_label"].unique()
    x = np.arange(len(sizes))  # позиция групп

    bar_width = 0.35

    for i, method in enumerate(methods):
        sub = data[data["method"] == method].sort_values("size_bytes")
        MBps = sub["bytes_per_second"] / 1_000_000
        plt.bar(x + i * bar_width, MBps, width=bar_width, label=method)

    plt.xticks(x + bar_width / 2, sizes)
    plt.xlabel("Размер данных")
    plt.ylabel("Пропускная способность (MB/s)")
    plt.title(title)
    plt.legend()
    plt.grid(True, axis="y")
    plt.tight_layout()
    plt.savefig(filename)
    plt.close()


plot_throughput_bar(substring, "Substring — пропускная способность", "substring_throughput.png")
plot_throughput_bar(brackets, "Bracket — пропускная способность", "bracket_throughput.png")
