import subprocess
import re
import os
from PyQt6.QtWidgets import QLabel, QWidget, QComboBox, QSlider, QPushButton, QMessageBox
from PyQt6.QtGui import QPixmap, QFont
from PyQt6.QtCore import Qt, QTimer


class DisplayManager:
    def __init__(self, parent):
        self.parent = parent
        self.refresh_timer = QTimer()
        self.refresh_timer.setInterval(1000)
        self.refresh_timer.timeout.connect(self.auto_refresh)
        self.init_display()

    def detect_theme_image(self):
        base_path = os.path.expanduser("~/Elysia/assets/")
        try:
            output = subprocess.check_output(
                ["gsettings", "get", "org.gnome.desktop.interface", "color-scheme"]
            ).decode().strip()
            return os.path.join(base_path, "assets/light.png") if "prefer-light" in output else os.path.join(base_path, "assets/dark.png")
        except Exception:
            return os.path.join(base_path, "assets/light.png")

    def init_display(self):
        self.desktop_label = QLabel("", self.parent)
        self.desktop_label.setFont(QFont("ElysiaOSNew", 16, QFont.Weight.Bold))
        self.desktop_label.setStyleSheet("color: white; background: transparent; border: none; margin: 0px; padding: 0px")
        self.desktop_label.move(760, 0)
        self.desktop_label.setVisible(False)

        self.desktop_image_label = QLabel(self.parent)
        self.desktop_image_label.setGeometry(650, 45, 300, 335)
        self.desktop_image_label.setAlignment(Qt.AlignmentFlag.AlignCenter)
        self.desktop_image_label.setVisible(False)

        self.display_box = QWidget(self.parent)
        self.display_box.setGeometry(500, 100, 600, 500)
        self.display_box.setStyleSheet("""
            background-color: rgba(255, 255, 255, 0.05);
            border: 2px solid rgba(255, 255, 255, 0.15);
            border-radius: 30px;
        """)
        self.display_box.setVisible(False)

        self.resolution_label = QLabel("Resolution", self.display_box)
        self.resolution_label.setFont(QFont("ElysiaOSNew", 14))
        self.resolution_label.setStyleSheet("color: white; background: transparent; border: none; margin: 0px; padding: 0px")
        self.resolution_label.move(220, 205)

        self.display_dropdown = QComboBox(self.display_box)
        self.display_dropdown.setGeometry(150, 240, 300, 40)
        self.display_dropdown.setStyleSheet("""
            QComboBox {
                background: rgba(255, 255, 255, 0.2);
                color: white;
                border: 1px solid rgba(255, 255, 255, 0.25);
                border-radius: 21px;
                padding: 5px;
                font-size: 16px;
            }
            QComboBox:hover {
                background: rgba(255, 255, 255, 0.3);
            }
            QComboBox QAbstractItemView {
                background: rgba(0, 0, 0, 0.2);
                border: none;
                color: white;
                border-radius: 21px;
                padding: 5px;
                selection-background-color: rgba(255, 255, 255, 0.2);
            }
        """)

        self.brightness_label = QLabel("Brightness", self.display_box)
        self.brightness_label.setFont(QFont("ElysiaOSNew", 14))
        self.brightness_label.setStyleSheet("color: white; background: transparent; border: none; margin: 0px; padding: 0px")
        self.brightness_label.move(220, 300)

        self.brightness_slider = QSlider(Qt.Orientation.Horizontal, self.display_box)
        self.brightness_slider.setGeometry(100, 340, 400, 30)
        self.brightness_slider.setMinimum(1)
        self.brightness_slider.setMaximum(100)
        self.brightness_slider.setStyleSheet("""
            QSlider::horizontal {
                background: transparent;
                border: none;
                padding: 0px;
                margin: 0px;
            }
            QSlider::groove:horizontal {
                height: 8px;
                background: transparent;
                border: none;
                padding: 0px;
                margin: 0px;
            }
            QSlider::handle:horizontal {
                background: pink;
                border: 1px solid white;
                width: 18px;
                margin: -6px 0;
                border-radius: 9px;
            }
            QSlider::sub-page:horizontal {
                background: rgba(255, 255, 255, 0.25);
                border-radius: 5px;
            }
            QSlider::add-page:horizontal {
                background: rgba(255, 255, 255, 0.05);
                border-radius: 5px;
            }
        """)
        self.brightness_slider.valueChanged.connect(self.set_brightness)

        self.apply_btn = QPushButton("Apply Changes", self.display_box)
        self.apply_btn.setGeometry(200, 400, 200, 50)
        self.apply_btn.setStyleSheet("""
            QPushButton {
                background-color: #e5a7c6;
                color: white;
                font-weight: bold;
                border: none;
                padding: 6px 12px;
                letter-spacing: 1px;
                border-radius: 10px;
                font-size: 16px;
            }
            QPushButton:hover {
                background-color: #edcee3;
            }
        """)
        self.apply_btn.clicked.connect(self.apply_display_change)

    def show(self):
        self.update_display_dropdown()
        self.update_brightness_slider()
        self.update_theme_image()

        self.desktop_image_label.setVisible(True)
        self.desktop_label.setVisible(True)
        self.display_box.setVisible(True)

        self.parent.hide_tiles()
        self.parent.show_back_button()

        self.refresh_timer.start()

    def hide(self):
        self.refresh_timer.stop()
        self.display_box.setVisible(False)
        self.desktop_image_label.setVisible(False)
        self.desktop_label.setVisible(False)
        self.parent.hide_back_button()
        self.parent.show_tiles()

    def auto_refresh(self):
        if self.display_box.isVisible():
            self.update_brightness_slider()
            self.update_display_dropdown()
            self.update_theme_image()

    def update_display_dropdown(self):
        modes = self.get_monitor_modes()
        current_items = [self.display_dropdown.itemText(i) for i in range(self.display_dropdown.count())]
        if set(modes) != set(current_items):
            self.display_dropdown.clear()
            self.display_dropdown.addItems(modes)

        current_mode = None
        try:
            with open("/home/matsuko/.config/hypr/hyprland.conf", "r") as f:
                for line in f:
                    if line.startswith("monitor") and "@" in line:
                        match = re.search(r"\d{3,4}x\d{3,4}@\d{2,3}", line)
                        if match:
                            current_mode = match.group(0)
                        self.parent.monitor_name = line.split()[1]
                        break
        except Exception:
            self.parent.monitor_name = None

        if current_mode and current_mode in modes:
            self.display_dropdown.setCurrentText(current_mode)

    def update_theme_image(self):
        image_path = self.detect_theme_image()
        self.desktop_image_label.setPixmap(QPixmap(image_path).scaled(
            300, 500, Qt.AspectRatioMode.KeepAspectRatio, Qt.TransformationMode.SmoothTransformation))

    def get_monitor_modes(self):
        try:
            output = subprocess.check_output(["hyprctl", "monitors"], stderr=subprocess.STDOUT).decode()
        except Exception:
            return []
        return list(sorted(set(re.findall(r"\d{3,4}x\d{3,4}@\d{2,3}", output))))

    def set_brightness(self, value):
        try:
            subprocess.run(["brightnessctl", "set", f"{value}%"], check=True)
        except Exception as e:
            QMessageBox.warning(self.parent, "Error", f"Failed to set brightness:\n{e}")

    def update_brightness_slider(self):
        try:
            output = subprocess.check_output(["brightnessctl", "g"]).decode().strip()
            max_output = subprocess.check_output(["brightnessctl", "m"]).decode().strip()
            percent = int(int(output) / int(max_output) * 100)
            if self.brightness_slider.value() != percent:
                self.brightness_slider.setValue(percent)
        except Exception:
            self.brightness_slider.setValue(50)

    def get_current_monitor_line(self):
        path = "/home/matsuko/.config/hypr/hyprland.conf"
        try:
            with open(path, "r") as f:
                lines = f.readlines()
        except FileNotFoundError:
            return None, []

        for i, line in enumerate(lines):
            if line.startswith("monitor") and "@" in line:
                return i, lines
        return None, lines

    def apply_display_change(self):
        new_mode = self.display_dropdown.currentText()
        index, lines = self.get_current_monitor_line()
        if index is None:
            QMessageBox.warning(self.parent, "Error", "Could not find monitor config line.")
            return

        lines[index] = re.sub(r"\d{3,4}x\d{3,4}@\d{2,3}", new_mode, lines[index])
        try:
            with open("/home/matsuko/.config/hypr/hyprland.conf", "w") as f:
                f.writelines(lines)
        except Exception as e:
            QMessageBox.warning(self.parent, "Error", f"Failed to write file:\n{e}")
            return

        QMessageBox.information(self.parent, "Success", f"Display mode changed to {new_mode}.\nRestart Hyprland for changes to apply.")
