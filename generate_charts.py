import json
import matplotlib.pyplot as plt
import pandas as pd


def load_data(json_file):
    """Load JSON data from a file."""
    with open(json_file, "r") as file:
        data = json.load(file)
    return data


def extract_metrics(data):
    """Extract metrics for each iteration and run."""
    metrics = {
        "iteration": [],
        "run": [],
        "execution_time": [],
        "cpu_usage": [],
        "power_consumed": [],
        "power_per_second": []
    }
    
    for iteration in data:
        iteration_num = iteration["iteration"]
        for run_data in iteration["runs"]:
            if "run" in run_data:  # Skip average records
                run = run_data["run"]
                execution_time = run_data["execution_time"]
                cpu_usage = run_data["cpu_usage"]
                power_consumed = run_data["power_consumed"] * 1000  # Convert W to mW
                power_per_second = power_consumed / execution_time if execution_time > 0 else 0

                metrics["iteration"].append(iteration_num)
                metrics["run"].append(run)
                metrics["execution_time"].append(execution_time)
                metrics["cpu_usage"].append(cpu_usage * 100)  # Convert to %
                metrics["power_consumed"].append(power_consumed)
                metrics["power_per_second"].append(power_per_second)
    
    return pd.DataFrame(metrics)


def plot_graphs(df):
    """Generate plots for metrics."""
    # Plot Power Consumption
    plt.figure(figsize=(10, 6))
    for iteration in df["iteration"].unique():
        subset = df[df["iteration"] == iteration]
        plt.plot(subset["run"], subset["power_consumed"], label=f"Iteration {iteration}")
    plt.xlabel("Run")
    plt.ylabel("Power Consumption (mW)")
    plt.title("Power Consumption Across Runs")
    plt.legend()
    plt.grid()
    plt.show()

    # Plot CPU Usage
    plt.figure(figsize=(10, 6))
    for iteration in df["iteration"].unique():
        subset = df[df["iteration"] == iteration]
        plt.plot(subset["run"], subset["cpu_usage"], label=f"Iteration {iteration}")
    plt.xlabel("Run")
    plt.ylabel("CPU Usage (%)")
    plt.title("CPU Usage Across Runs")
    plt.legend()
    plt.grid()
    plt.show()

    # Plot Power Consumption per Second (mW/s)
    plt.figure(figsize=(10, 6))
    for iteration in df["iteration"].unique():
        subset = df[df["iteration"] == iteration]
        plt.plot(subset["run"], subset["power_per_second"], label=f"Iteration {iteration}")
    plt.xlabel("Run")
    plt.ylabel("Power Consumption per Second (mW/s)")
    plt.title("Power Consumption per Second Across Runs")
    plt.legend()
    plt.grid()
    plt.show()


def analyze_efficiency(df):
    """Analyze the relationship between execution time and power consumption per second."""
    avg_power_per_second = df.groupby("iteration")["power_per_second"].mean()
    print("Average Power Consumption per Second (mW/s) by Iteration:")
    print(avg_power_per_second)

    plt.figure(figsize=(10, 6))
    plt.plot(avg_power_per_second.index, avg_power_per_second.values, marker="o")
    plt.xlabel("Iteration")
    plt.ylabel("Average Power Consumption per Second (mW/s)")
    plt.title("Efficiency Evolution: Power Consumption per Second")
    plt.grid()
    plt.show()


if __name__ == "__main__":
    # Replace with your JSON file path
    json_file = "results.json"
    data = load_data(json_file)
    df = extract_metrics(data)

    plot_graphs(df)
    analyze_efficiency(df)
