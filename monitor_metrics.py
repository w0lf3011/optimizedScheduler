import docker
import psutil
import time
import sys

def monitor_container_metrics(container_id, duration, output_file):
    client = docker.from_env()
    container = client.containers.get(container_id)

    with open(output_file, 'w') as f:
        f.write("timestamp,cpu_usage,ram_usage,estimated_energy_consumed\n")
        
        start_time = time.time()
        while time.time() - start_time < duration:
            stats = container.stats(stream=False)
            
            # Calcul de l'utilisation du CPU
            cpu_delta = stats['cpu_stats']['cpu_usage']['total_usage'] - stats['precpu_stats']['cpu_usage']['total_usage']
            system_delta = stats['cpu_stats']['system_cpu_usage'] - stats['precpu_stats']['system_cpu_usage']
            cpu_usage = (cpu_delta / system_delta) * len(stats['cpu_stats']['cpu_usage']['percpu_usage']) * 100.0

            # Calcul de l'utilisation de la RAM
            ram_usage = stats['memory_stats']['usage'] / (1024 * 1024)  # En Mo

            # Estimation de l'énergie consommée (simplifiée ici comme fonction du CPU)
            estimated_energy_consumed = cpu_usage * 0.001  # Exemple de coefficient pour une estimation

            # Enregistrer les métriques
            timestamp = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())
            f.write(f"{timestamp},{cpu_usage:.2f},{ram_usage:.2f},{estimated_energy_consumed:.2f}\n")

            time.sleep(1)  # Intervalle de mesure (1 seconde)

    print(f"Metrics saved to {output_file}")

if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Usage: python monitor_metrics.py <container_id> <duration_in_seconds> <output_file>")
        sys.exit(1)
    
    container_id = sys.argv[1]
    duration = int(sys.argv[2])
    output_file = sys.argv[3]

    monitor_container_metrics(container_id, duration, output_file)
