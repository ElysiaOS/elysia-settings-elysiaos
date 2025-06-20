import subprocess
from PyQt6.QtWidgets import QLabel, QWidget
from PyQt6.QtGui import QFont
from PyQt6.QtCore import Qt, QTimer


class BatteryManager:
    def __init__(self, parent):
        self.parent = parent
        self.labels = []
        self.battery_device = self.find_battery_device()
        self.timer = QTimer()
        self.timer.timeout.connect(self.update_info)
        self.init_battery()

    def find_battery_device(self):
        try:
            output = subprocess.check_output(["upower", "-e"]).decode()
            for line in output.strip().splitlines():
                if "battery_" in line:
                    return line.strip()
        except Exception:
            return None

    def parse_battery_info(self, device):
        try:
            output = subprocess.check_output(["upower", "-i", device]).decode()
        except Exception:
            return {}

        info = {}
        wanted_keys = [
            "state", "warning-level", "energy-full", "energy-full-design",
            "energy-rate", "voltage", "charge-cycles", "percentage",
            "capacity", "technology"
        ]
        for line in output.splitlines():
            if ":" not in line:
                continue
            key, value = line.strip().split(":", 1)
            key = key.strip()
            value = value.strip()
            if key in wanted_keys:
                info[key] = value
        return info

    def init_battery(self):
        self.battery_box = QWidget(self.parent)
        self.battery_box.setGeometry(500, 100, 600, 500)
        self.battery_box.setStyleSheet("""
            background-color: rgba(255, 255, 255, 0.05);
            border: 2px solid rgba(255, 255, 255, 0.15);
            border-radius: 30px;
        """)
        self.battery_box.setVisible(False)

        # Title Label
        title = QLabel("BATTERY", self.battery_box)
        title.setFont(QFont("ElysiaOSNew", 20, QFont.Weight.Bold))
        title.setStyleSheet("color: white; background: transparent; border: none; margin: 0px; padding: 0px")
        title.setAlignment(Qt.AlignmentFlag.AlignCenter)
        title.setGeometry(200, 20, 200, 40)
        title.setVisible(True)

        if not self.battery_device:
            label = QLabel("No battery device found.", self.battery_box)
            label.setFont(QFont("ElysiaOSNew", 14))
            label.setStyleSheet("color: white; background: transparent; border: none; margin: 0px; padding: 0px")
            label.setGeometry(80, 230, 440, 40)
            label.setWordWrap(True)
            label.setVisible(True)
            self.labels.append(label)
            return

        keys_to_display = [
            "state", "warning-level", "energy-full", "energy-full-design",
            "energy-rate", "voltage", "charge-cycles", "percentage",
            "capacity", "technology"
        ]

        y = 80
        for key in keys_to_display:
            label = QLabel("", self.battery_box)
            label.setFont(QFont("ElysiaOSNew", 14))
            label.setStyleSheet("color: white; background: transparent; border: none; margin: 0px; padding: 0px")
            label.setGeometry(80, y, 440, 30)
            label.setWordWrap(True)
            label.setVisible(True)
            self.labels.append(label)
            y += 35

        self.update_info()

    def update_info(self):
        if not self.battery_device:
            return

        battery_info = self.parse_battery_info(self.battery_device)
        keys = [
            "state", "warning-level", "energy-full", "energy-full-design",
            "energy-rate", "voltage", "charge-cycles", "percentage",
            "capacity", "technology"
        ]

        for i, key in enumerate(keys):
            value = battery_info.get(key, "N/A")
            self.labels[i].setText(f"{key.capitalize()}: {value}")

    def show(self):
        self.parent.hide_tiles()
        self.battery_box.setVisible(True)
        self.parent.show_back_button()
        self.update_info()
        self.timer.start(1000)

    def hide(self):
        self.timer.stop()
        self.battery_box.setVisible(False)
        self.parent.hide_back_button()
        self.parent.show_tiles()
