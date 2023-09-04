import matplotlib.pyplot as plt
import numpy as np


def plot_method(data, method_name):
    # create a stack-bar plot for the method. each stack-bar in plot is consist of the worker and host time
    # the x-axis is the number of elements in 2^i scale
    # the y-axis is the time in ms

    data_elements = range(16, 16 + len(data))
    worker_time = ["{:.2f}".format(float(data[i].split(',')[1])) for i in range(len(data))]
    host_time = ["{:.2f}".format(float(data[i].split(',')[0])) for i in range(len(data))]

    # convert to ndarray of float only (4 precision)
    worker_time = np.array(worker_time, dtype=float)
    host_time = np.array(host_time, dtype=float)

    # create a dictionary for the stack-bar plot
    weight_counts = {
        "Worker Time": worker_time,
        "Host Time": host_time,
    }

    width = 0.5

    fig, ax = plt.subplots()
    bottom = np.zeros(len(data))

    for type, weight_count in weight_counts.items():
        p = ax.bar(data_elements, weight_count, width, label=type, bottom=bottom,
                   color='#4287f5' if type == "Worker Time" else '#96173f')
        bottom += weight_count

        ax.bar_label(p, label_type='center')

    ax.set_title(f"Time vs Number of Elements ({method_name})")
    ax.legend(loc="upper left")

    # set name for x and y-axis
    ax.set_xlabel("Number of Elements (2^i)")
    ax.set_ylabel("Time (ms)")

    # # set label for each bar
    # for rect in ax.patches:
    #     auto_label(ax, rect)

    fig.savefig(f'../../results/dataDistribution/{method_name}.png')
    fig.show()


def compare_methods(data1, data2, method1_name, method2_name, parameter, index, base=0):
    # create a plot which compares method 1 and 2
    data_elements = range(16, 16 + len(data))
    total_time_1 = ["{:.2f}".format(float(data1[i].split(',')[index])) for i in range(len(data))]
    total_time_2 = ["{:.2f}".format(float(data2[i].split(',')[index])) for i in range(len(data))]

    # convert to ndarray of float only
    total_time_1 = np.array(total_time_1, dtype=float)
    total_time_2 = np.array(total_time_2, dtype=float)

    # create a simple xy plot
    fig, ax = plt.subplots()
    ax.plot(data_elements, total_time_1, marker='o', linestyle='--', color='#e76680', label=method1_name)
    ax.plot(data_elements, total_time_2, marker='o', linestyle='--', color='#4287f5', label=method2_name)

    ax.set_title(f"Time vs Number of Elements ({parameter} - {method1_name} vs {method2_name})")
    ax.set_ylabel("Time (ms)")
    ax.set_xlabel("Number of Elements (2^i)")

    # scale the y-axis to log
    if base != 0:
        ax.set_yscale('log', base=base)

    ax.legend()

    fig.savefig(f'../../results/dataDistribution/{parameter}_time_{method1_name}_vs_{method2_name}.png')

    fig.show()


# open two result files (merge_partition.csv and sort_partition.csv)
with open('../../results/dataDistribution/merge_partition.csv', 'r') as f:
    data = f.read()

with open('../../results/dataDistribution/two_step_sort.csv', 'r') as f:
    data2 = f.read()

# split CSV into lines
data = data.split('\n')
data2 = data2.split('\n')

plot_method(data, "Merge Partition")
plot_method(data2, "Two Step Sort")
compare_methods(data, data2, "Merge Partition", "Two Step Sort", "Worker", 1, 0)
compare_methods(data, data2, "Merge Partition", "Two Step Sort", "Host", 0, 10)
compare_methods(data, data2, "Merge Partition", "Two Step Sort", "Total", 2, 10)
