import matplotlib.pyplot as plt
import numpy as np

with open('../results/buffer_size.csv', 'r') as f:
    data = f.read()

data = data.split('\n')

x = [0] * len(data)
y = [0] * len(data)

for i in range(len(data)):
    x[i] = 10 if i == 0 else x[i - 1] * 2
    y[i] = "{:.2f}".format(float((data[i].split(',')[0])))
    y[i] = float(y[i])

fig, ax = plt.subplots()
ax.plot(x, y, marker='o', linestyle='--', color='#e76680')  # Change the line color
ax.set_title('Time vs Buffer Size')
ax.set_ylabel('Time (ms)')
ax.set_xlabel('Buffer Size')

# Perform linear regression
coefficients = np.polyfit(x, y, 1)
poly = np.poly1d(coefficients)
x_fit = np.linspace(min(x), max(x), 100)
y_fit = poly(x_fit)
ax.plot(x_fit, y_fit, color='skyblue', label='Linear Fit')

slope, intercept = coefficients
slope_sci = "{:.2e}".format(slope)
intercept_sci = "{:.2e}".format(intercept)

ax.text(0.95, 0.05, f"Slope: {slope_sci}\nIntercept: {intercept_sci}", transform=ax.transAxes, fontsize=12, va='bottom',
        ha='right')

plt.legend()
plt.show()

fig.savefig('../results/min_chunk_size.png')

print("Slope:", slope_sci)
print("Intercept:", intercept_sci)
