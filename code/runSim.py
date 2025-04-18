import sys
import subprocess
import numpy as np


import matplotlib.pyplot as plt


def run_simulation(num_peer, z0, z1, Ttx, Tk, iterations=3):
    print(f"Running simulation with num_peer={num_peer}, Ttx={Ttx}, Tk={Tk}, z0={z0}, z1={z1}")
    outputs = [subprocess.run(["./run", str(num_peer), str(z0), str(z1), str(Ttx), str(Tk), "30", "--ratio"], 
                              capture_output=True, text=True).stdout for _ in range(iterations)]
    
    def extract_value(output, key):
        for line in output.splitlines():
            # print(line)
            if key in line:
                value = line.split()[4]
                return np.nan if value == "-nan" else float(value)
        return np.nan  # If the key is not found, return NaN
    
    categories = ["Fast High CPU", "Fast Low CPU", "Slow High CPU", "Slow Low CPU"]
    results = {}
    
    for category in categories:
        values = [extract_value(output, category) for output in outputs]
        filtered_values = [v for v in values if not np.isnan(v)]
        results[category] = np.mean(filtered_values) if filtered_values else np.nan
    
    return results

def main():
    if len(sys.argv) != 2:
        print("Usage: python3 script.py <parameter>")
        print("Parameters: num_peers, Ttx, Tk, z0, z1")
        sys.exit(1)
    
    parameter = sys.argv[1]
    param_values = {
        "num_peers": list(range(50, 160, 10)),
        "z0": list(range(10, 110, 10)),
        "z1": list(range(10, 110, 10)),
        "Ttx": [1, 3, 6, 10, 15, 21, 28, 36, 45, 55],
        "Tk": list(range(350, 800, 50))
    }
    
    if parameter not in param_values:
        print("Invalid parameter.")
        sys.exit(1)
    
    results_dict = {"Fast High CPU": [], "Fast Low CPU": [], "Slow High CPU": [], "Slow Low CPU": []}
    
    for val in param_values[parameter]:
        if parameter == "num_peers":
            results = run_simulation(val, 50, 50, 10, 600)
        elif parameter == "z0":
            results = run_simulation(100, val, 50, 10, 600)
        elif parameter == "z1":
            results = run_simulation(100, 50, val, 10, 600)
        elif parameter == "Ttx":
            results = run_simulation(100, 50, 50, val, 600)
        elif parameter == "Tk":
            results = run_simulation(100, 50, 50, 10, val)
        for key in results_dict:
            results_dict[key].append(results[key])
    
    for key, values in results_dict.items():
        print(f"{key}: {values}")

    fast_high_cpu = results_dict["Fast High CPU"]
    fast_low_cpu = results_dict["Fast Low CPU"]
    slow_high_cpu = results_dict["Slow High CPU"]
    slow_low_cpu = results_dict["Slow Low CPU"]
    x_axis = param_values[parameter]

    plt.plot(x_axis, fast_high_cpu, label='Fast High CPU')
    plt.plot(x_axis, fast_low_cpu, label='Fast Low CPU')
    plt.plot(x_axis, slow_high_cpu, label='Slow High CPU')
    plt.plot(x_axis, slow_low_cpu, label='Slow Low CPU')

    plt.xlabel(f"parameter: {parameter}")
    plt.ylabel('Block Ratio')
    plt.title('')
    plt.legend()
    plt.show()
    

if __name__ == "__main__":
    main()
