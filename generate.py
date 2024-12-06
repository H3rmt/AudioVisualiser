import math

count = 32

long_grow_decrease = 17
initial_addition = 1.5
intial_spike_decrease = 2.5
offset = -1.4

def calc(x: int) -> float:
    return math.exp(x / long_grow_decrease) + (initial_addition / (x + intial_spike_decrease)) + offset
     
h_values = [calc(x) for x in range(count)]

# Print the array for C code
c_array = ", ".join(f"{h_val:.6f}" for h_val in h_values)
print(f"double h_values[{count}] = {{{c_array}}};") 