import matplotlib.pyplot as plt
import numpy as np

labels = ['1', '2', '4', '6', '12']
baseline = [0] * 5
omp_task = [0] * 5
threadpool = [0] * 5

# read CSV from  ../results/threads_num.csv
with open('../../../results/sort/quicksort/threads_num.csv', 'r') as f:
    data = f.read()

# split CSV into lines
data = data.split('\n')

for i in range(len(data)):
    data[i] = data[i].split(',')

    baseline[i] = "{:.2f}".format(float(data[i][0]))
    omp_task[i] = "{:.2f}".format(float(data[i][1]))
    threadpool[i] = "{:.2f}".format(float(data[i][2]))


# convert each element to float
baseline = [float(i) for i in baseline]
omp_task = [float(i) for i in omp_task]
threadpool = [float(i) for i in threadpool]

# zero baseline except the first one
for i in range(1, len(baseline)):
    baseline[i] = 0

# zero first element of omp_task and threadpool except the last one
omp_task[0] = 0
threadpool[0] = 0

# plot
x = np.arange(len(labels))
width = 0.5 / 2
margin = 0.12 / 3

fig, ax = plt.subplots()
rects1 = ax.bar(x - width, baseline, width, label='Baseline', color='#96173f')
rects2 = ax.bar(x, omp_task, width, label='OpenMP Task', color='#72de68')
rects3 = ax.bar(x + width + margin, threadpool, width, label='Threadpool', color='#4287f5')

ax.set_title('Time vs Number of threads (n = 1,000,000)')
ax.set_ylabel('Time (ms)')
ax.set_xlabel('Number of threads')

ax.set_xticks(x)
ax.set_xticklabels(labels)
ax.legend()


def auto_label(rects):
    for rect in rects:
        height = rect.get_height()
        if height != 0:
            ax.annotate('{}'.format(height), xy=(rect.get_x() + rect.get_width() / 3,
                                                 height), xytext=(0, 3),
                        textcoords="offset points", ha='center', va='bottom')


auto_label(rects1)
auto_label(rects2)
auto_label(rects3)

fig.tight_layout()

plt.show()

# save to ../results/threads_num.png
fig.savefig('../../../results/sort/quicksort/threads_num.png')
