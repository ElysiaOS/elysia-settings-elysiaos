import subprocess
from PyQt6.QtWidgets import (
    QWidget, QLabel, QPushButton, QVBoxLayout, QHBoxLayout,
    QListWidget, QListWidgetItem, QLineEdit, QInputDialog,
    QMessageBox, QFrame
)
from PyQt6.QtCore import QTimer, Qt
from PyQt6.QtGui import QFont


class NetworkManager:
    def __init__(self, parent):
        self.parent = parent
        self.init_network_ui()

    def init_network_ui(self):
        self.network_box = QWidget(self.parent)
        self.network_box.setGeometry(500, 100, 600, 500)
        self.network_box.setStyleSheet("""
            background-color: rgba(255, 255, 255, 0.05);
            border: 2px solid rgba(255, 255, 255, 0.15);
            border-radius: 26px;
        """)
        self.network_box.setVisible(False)

        self.layout = QVBoxLayout(self.network_box)
        self.layout.setContentsMargins(30, 30, 30, 30)
        self.layout.setSpacing(15)

        self.current_label = QLabel("Connected to: Loading...", self.network_box)
        self.current_label.setFont(QFont("ElysiaOSNew", 16, QFont.Weight.Bold))
        self.current_label.setStyleSheet("""
            color: white;
            background: transparent;
            border: none;
            margin: 0px;
            padding: 0px;
        """)
        self.current_label.setAlignment(Qt.AlignmentFlag.AlignCenter)
        self.layout.addWidget(self.current_label)

        self.disconnect_button = QPushButton("Disconnect", self.network_box)
        self.disconnect_button.setFixedHeight(40)
        self.disconnect_button.setStyleSheet("""
            QPushButton {
                background-color: #ff5555;
                color: white;
                border: none;
                border-radius: 10px;
                font-weight: bold;
                font-size: 14px;
            }
            QPushButton:hover {
                background-color: #ff7777;
            }
        """)
        self.disconnect_button.clicked.connect(self.disconnect_wifi)
        self.layout.addWidget(self.disconnect_button, alignment=Qt.AlignmentFlag.AlignCenter)

        self.enable_button = QPushButton("Enable Wi-Fi", self.network_box)
        self.enable_button.setFixedHeight(40)
        self.enable_button.setStyleSheet("""
            QPushButton {
                background-color: #55ff55;
                color: white;
                border: none;
                border-radius: 10px;
                font-weight: bold;
                font-size: 14px;
            }
            QPushButton:hover {
                background-color: #77ff77;
            }
        """)
        self.enable_button.clicked.connect(self.enable_wifi)
        self.layout.addWidget(self.enable_button, alignment=Qt.AlignmentFlag.AlignCenter)
        self.enable_button.setVisible(False)

        self.list_label = QLabel("Available Wi-Fi Networks:", self.network_box)
        self.list_label.setFont(QFont("ElysiaOSNew", 14))
        self.list_label.setStyleSheet("""
            color: white;
            background: transparent;
            border: none;
            margin: 0px;
            padding: 0px;
        """)
        self.layout.addWidget(self.list_label, alignment=Qt.AlignmentFlag.AlignCenter)

        self.wifi_container = QFrame(self.network_box)
        self.wifi_container.setStyleSheet("""
            QFrame {
                background-color: rgba(255, 255, 255, 0.05);
                border: 2px solid rgba(255, 255, 255, 0.1);
                border-radius: 20px;
            }
        """)

        wifi_layout = QVBoxLayout(self.wifi_container)
        wifi_layout.setContentsMargins(10, 10, 10, 10)

        self.wifi_list = QListWidget(self.wifi_container)
        self.wifi_list.setStyleSheet("""
            QListWidget {
                background: transparent;
                border: none;
                margin: 0px;
                padding: 0px;
                font-family: ElysiaOSNew;
            }
        """)
        self.wifi_list.setFrameStyle(QFrame.Shape.NoFrame)
        wifi_layout.addWidget(self.wifi_list)
        self.layout.addWidget(self.wifi_container)

        self.refresh_timer = QTimer(self.network_box)
        self.refresh_timer.timeout.connect(self.refresh_wifi)
        self.refresh_timer.start(5000)

        self.refresh_wifi()

    def show(self):
        self.refresh_wifi()
        self.network_box.setVisible(True)
        self.parent.hide_tiles()
        self.parent.show_back_button()
        self.parent.current_view = "network"

    def hide(self):
        self.network_box.setVisible(False)
        self.parent.show_tiles()
        self.parent.hide_back_button()

    def refresh_wifi(self):
        try:
            wifi_enabled = self.is_wifi_enabled()
            result = subprocess.check_output(
                ['nmcli', '-t', '-f', 'active,ssid', 'dev', 'wifi'], encoding='utf-8'
            ).strip().splitlines()
            connected = next((line.split(":")[1] for line in result if line.startswith("yes:")), "None")
        except Exception:
            wifi_enabled = False
            connected = "Error"
            result = []

        self.current_label.setText(f"Connected to: {connected}")
        self.wifi_list.clear()

        # Toggle buttons based on Wi-Fi state
        if not wifi_enabled or connected in ["None", "", "Error"]:
            self.disconnect_button.setVisible(False)
            self.enable_button.setVisible(True)
        else:
            self.disconnect_button.setVisible(True)
            self.enable_button.setVisible(False)

        if not wifi_enabled:
            return  # Skip network scan if Wi-Fi is disabled

        saved_ssids = self.get_saved_connections()

        for line in result:
            active, ssid = line.split(":")
            if active == "yes":
                continue

            item_widget = QWidget()
            item_layout = QHBoxLayout(item_widget)
            item_layout.setContentsMargins(10, 5, 10, 5)

            label = QLabel(ssid)
            label.setFont(QFont("ElysiaOSNew", 12))
            item_widget.setStyleSheet("""
                color: white;
                background: transparent;
                border: none;
                margin: 0px;
                padding: 0px;
            """)

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
            connect_btn.clicked.connect(lambda _, s=ssid: self.connect_to_wifi(s))

            item_layout.addWidget(label)
            item_layout.addStretch()
            item_layout.addWidget(connect_btn)

            if ssid in saved_ssids:
                forget_btn = QPushButton("Forget")
                forget_btn.setFixedWidth(100)
                forget_btn.setStyleSheet("""
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
                forget_btn.clicked.connect(lambda _, s=ssid: self.forget_wifi(s))
                item_layout.addWidget(forget_btn)

            list_item = QListWidgetItem(self.wifi_list)
            list_item.setSizeHint(item_widget.sizeHint())
            self.wifi_list.addItem(list_item)
            self.wifi_list.setItemWidget(list_item, item_widget)

    def connect_to_wifi(self, ssid):
        if self.is_saved_connection(ssid):
            try:
                subprocess.run(['nmcli', 'connection', 'up', ssid], check=True)
                self.current_label.setText(f"Connected to: {ssid}")
            except subprocess.CalledProcessError:
                QMessageBox.warning(self.parent, "Error", f"Failed to connect to saved network {ssid}.")
        else:
            password, ok = QInputDialog.getText(
                self.network_box,
                "Wi-Fi Password",
                f"Enter password for {ssid}:",
                echo=QLineEdit.EchoMode.Password
            )
            if ok and password:
                try:
                    subprocess.run(
                        ['nmcli', 'device', 'wifi', 'connect', ssid, 'password', password],
                        check=True
                    )
                    self.current_label.setText(f"Connected to: {ssid}")
                except subprocess.CalledProcessError:
                    QMessageBox.warning(self.parent, "Error", f"Failed to connect to {ssid}.")

        self.refresh_wifi()

    def forget_wifi(self, ssid):
        if self.is_saved_connection(ssid):
            try:
                subprocess.run(['nmcli', 'connection', 'delete', ssid], check=True)
                QMessageBox.information(self.parent, "Success", f"Forgot network {ssid}.")
            except subprocess.CalledProcessError:
                QMessageBox.warning(self.parent, "Error", f"Failed to forget network {ssid}.")
        else:
            QMessageBox.information(self.parent, "Info", f"No saved connection for {ssid}.")
        self.refresh_wifi()

    def is_saved_connection(self, ssid):
        return ssid in self.get_saved_connections()

    def get_saved_connections(self):
        try:
            return subprocess.check_output(
                ['nmcli', '-t', '-f', 'NAME', 'connection', 'show'], encoding='utf-8'
            ).strip().splitlines()
        except Exception:
            return []

    def disconnect_wifi(self):
        try:
            subprocess.run(['nmcli', 'networking', 'off'], check=True)
            subprocess.run(['nmcli', 'networking', 'on'], check=True)
            self.current_label.setText("Disconnected.")
        except subprocess.CalledProcessError:
            self.current_label.setText("Failed to disconnect.")
        self.refresh_wifi()

    def enable_wifi(self):
        try:
            subprocess.run(['nmcli', 'radio', 'wifi', 'on'], check=True)
            self.current_label.setText("Wi-Fi Enabled.")
        except subprocess.CalledProcessError:
            self.current_label.setText("Failed to enable Wi-Fi.")
        self.refresh_wifi()

    def is_wifi_enabled(self):
        try:
            output = subprocess.check_output(['nmcli', 'radio', 'wifi'], encoding='utf-8').strip()
            return output.lower() == "enabled"
        except Exception:
            return False
