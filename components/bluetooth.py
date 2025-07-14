import subprocess
from PyQt6.QtWidgets import (
    QWidget, QLabel, QPushButton, QVBoxLayout, QHBoxLayout,
    QListWidget, QListWidgetItem, QMessageBox, QFrame
)
from PyQt6.QtCore import QTimer, Qt
from PyQt6.QtGui import QFont


class BluetoothManager:
    def __init__(self, parent):
        self.parent = parent
        self.scanning = False
        self.init_bluetooth_ui()

    def init_bluetooth_ui(self):
        self.bluetooth_box = QWidget(self.parent)
        self.bluetooth_box.setGeometry(500, 100, 600, 500)
        self.bluetooth_box.setStyleSheet("""
            background-color: rgba(255, 255, 255, 0.05);
            border: 2px solid rgba(255, 255, 255, 0.15);
            border-radius: 26px;
        """)
        self.bluetooth_box.setVisible(False)

        self.layout = QVBoxLayout(self.bluetooth_box)
        self.layout.setContentsMargins(30, 30, 30, 30)
        self.layout.setSpacing(15)

        self.status_label = QLabel("Bluetooth: Loading...", self.bluetooth_box)
        self.status_label.setFont(QFont("ElysiaOSNew", 16, QFont.Weight.Bold))
        self.status_label.setStyleSheet("color: white; background: transparent; border: none;")
        self.status_label.setAlignment(Qt.AlignmentFlag.AlignCenter)
        self.layout.addWidget(self.status_label)

        button_layout = QHBoxLayout()
        self.toggle_button = QPushButton("Toggle Bluetooth", self.bluetooth_box)
        self.scan_button = QPushButton("Scan for Devices", self.bluetooth_box)

        for btn in (self.toggle_button, self.scan_button):
            btn.setFixedHeight(40)
            btn.setStyleSheet("""
                QPushButton {
                    background-color: #e5a7c6;
                    color: white;
                    border: none;
                    border-radius: 10px;
                    font-weight: bold;
                    font-size: 14px;
                }
                QPushButton:hover {
                    background-color: #edcee3;
                }
            """)
        self.toggle_button.clicked.connect(self.toggle_bluetooth)
        self.scan_button.clicked.connect(self.toggle_scan)

        button_layout.addWidget(self.toggle_button)
        button_layout.addWidget(self.scan_button)
        self.layout.addLayout(button_layout)

        self.list_label = QLabel("Available Bluetooth Devices:", self.bluetooth_box)
        self.list_label.setFont(QFont("ElysiaOSNew", 14))
        self.list_label.setStyleSheet("color: white; background: transparent; border: none;")
        self.layout.addWidget(self.list_label, alignment=Qt.AlignmentFlag.AlignCenter)

        self.device_container = QFrame(self.bluetooth_box)
        self.device_container.setStyleSheet("""
            QFrame {
                background-color: rgba(255, 255, 255, 0.05);
                border: 2px solid rgba(255, 255, 255, 0.1);
                border-radius: 20px;
            }
        """)

        device_layout = QVBoxLayout(self.device_container)
        device_layout.setContentsMargins(10, 10, 10, 10)

        self.device_list = QListWidget(self.device_container)
        self.device_list.setStyleSheet("""
            QListWidget {
                background: transparent;
                border: none;
                font-family: ElysiaOSNew;
            }
        """)
        self.device_list.setFrameStyle(QFrame.Shape.NoFrame)
        device_layout.addWidget(self.device_list)
        self.layout.addWidget(self.device_container)

        self.refresh_timer = QTimer(self.bluetooth_box)
        self.refresh_timer.timeout.connect(self.refresh_devices)
        self.refresh_timer.start(5000)

        self.refresh_devices()

    def show(self):
        self.refresh_devices()
        self.bluetooth_box.setVisible(True)
        self.parent.hide_tiles()
        self.parent.show_back_button()
        self.parent.current_view = "bluetooth"

    def hide(self):
        self.bluetooth_box.setVisible(False)
        self.parent.show_tiles()
        self.parent.hide_back_button()

    def toggle_bluetooth(self):
        state = self.get_bluetooth_power_state()
        try:
            subprocess.run(["bluetoothctl", "power", "off" if state else "on"], check=True)
        except subprocess.CalledProcessError:
            QMessageBox.warning(self.parent, "Error", "Failed to toggle Bluetooth.")
        self.refresh_devices()

    def get_bluetooth_power_state(self):
        try:
            output = subprocess.check_output(["bluetoothctl", "show"], encoding="utf-8")
            for line in output.splitlines():
                if "Powered:" in line:
                    return "yes" in line.lower()
        except Exception:
            return False

    def toggle_scan(self):
        if self.scanning:
            self.scan_button.setText("Scan for Devices")
            self.scanning = False
            return

        self.scan_button.setText("Scanning...")
        self.scan_button.setEnabled(False)
        self.scanning = True

        try:
            subprocess.run(["bluetoothctl", "--timeout", "3", "scan", "on"], check=True)
            subprocess.run(["bluetoothctl", "--timeout", "2", "scan", "off"], check=True)
        except subprocess.CalledProcessError as e:
            QMessageBox.warning(self.parent, "Error", f"Bluetooth scan failed:\n{e}")
        finally:
            self.scanning = False
            self.scan_button.setText("Scan for Devices")
            self.scan_button.setEnabled(True)
            self.refresh_devices()

    def refresh_devices(self):
        state = self.get_bluetooth_power_state()
        self.status_label.setText(f"Bluetooth: {'On' if state else 'Off'}")
        self.device_list.clear()

        if not state:
            return

        try:
            output = subprocess.check_output(["bluetoothctl", "devices"], encoding="utf-8").strip().splitlines()
        except subprocess.CalledProcessError:
            output = []

        seen = set()
        for line in output:
            if line.startswith("Device"):
                parts = line.split(" ", 2)
                if len(parts) >= 3:
                    mac = parts[1]
                    name = parts[2]
                    if mac in seen:
                        continue
                    seen.add(mac)

                    item_widget = QWidget()
                    item_layout = QHBoxLayout(item_widget)
                    item_layout.setContentsMargins(10, 5, 10, 5)

                    label = QLabel(name)
                    label.setFont(QFont("ElysiaOSNew", 12))
                    item_widget.setStyleSheet("color: white; background: transparent; border: none;")

                    connect_btn = QPushButton("Connect")
                    connect_btn.setFixedWidth(100)
                    connect_btn.setStyleSheet("""
                        QPushButton {
                            background-color: rgba(255, 255, 255, 0.15);
                            color: white;
                            border-radius: 8px;
                            padding: 5px;
                            font-weight: bold;
                        }
                        QPushButton:hover {
                            background-color: rgba(255, 255, 255, 0.25);
                        }
                    """)
                    connect_btn.clicked.connect(lambda _, m=mac: self.connect_device(m))

                    forget_btn = QPushButton("Forget")
                    forget_btn.setFixedWidth(100)
                    forget_btn.setStyleSheet("""
                        QPushButton {
                            background-color: rgba(255, 0, 0, 0.15);
                            color: white;
                            border-radius: 8px;
                            padding: 5px;
                            font-weight: bold;
                        }
                        QPushButton:hover {
                            background-color: rgba(255, 0, 0, 0.3);
                        }
                    """)
                    forget_btn.clicked.connect(lambda _, m=mac: self.forget_device(m))

                    item_layout.addWidget(label)
                    item_layout.addStretch()
                    item_layout.addWidget(connect_btn)
                    item_layout.addWidget(forget_btn)

                    list_item = QListWidgetItem(self.device_list)
                    list_item.setSizeHint(item_widget.sizeHint())
                    self.device_list.addItem(list_item)
                    self.device_list.setItemWidget(list_item, item_widget)

    def connect_device(self, mac_address):
        try:
            subprocess.run(["bluetoothctl", "pair", mac_address], check=True)
        except subprocess.CalledProcessError:
            pass  # Device may already be paired

        try:
            subprocess.run(["bluetoothctl", "connect", mac_address], check=True)
            QMessageBox.information(self.parent, "Success", f"Connected to {mac_address}.")
        except subprocess.CalledProcessError:
            QMessageBox.warning(self.parent, "Error", f"Failed to connect to {mac_address}.")

        self.refresh_devices()

    def forget_device(self, mac_address):
        try:
            subprocess.run(["bluetoothctl", "remove", mac_address], check=True)
            QMessageBox.information(self.parent, "Removed", f"Forgot device {mac_address}.")
        except subprocess.CalledProcessError:
            QMessageBox.warning(self.parent, "Error", f"Failed to forget device {mac_address}.")

        self.refresh_devices()
