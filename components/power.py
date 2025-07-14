import os
import subprocess
from PyQt6.QtWidgets import (
    QLabel, QWidget, QVBoxLayout, QHBoxLayout, QPushButton,
    QFrame, QStackedLayout
)
from PyQt6.QtGui import QPixmap, QFont
from PyQt6.QtCore import Qt


class PowerManager:
    def __init__(self, parent):
        self.parent = parent
        self.selected_mode = None
        self.mode_boxes = {}

        self.init_ui()

    def init_ui(self):
        self.power_box = QWidget(self.parent)
        self.power_box.setGeometry(455, 100, 700, 470)
        self.power_box.setStyleSheet("""
            background-color: rgba(255, 255, 255, 0.05);
            border: 2px solid rgba(255, 255, 255, 0.15);
            border-radius: 30px;
        """)
        self.power_box.setVisible(False)

        layout = QVBoxLayout(self.power_box)
        layout.setContentsMargins(150, 100, 150, 200)
        layout.setSpacing(25)

        power_label = QLabel("POWER options", self.power_box)
        power_label.setFont(QFont("ElysiaOSNew", 16))
        power_label.setStyleSheet("color: white; background: transparent; border: none; margin: 0px; padding: 0px")
        power_label.setGeometry(270, 0, 500, 160)

        mode_widget = QWidget(self.power_box)
        mode_widget.setGeometry(110, 320, 110, 250)
        mode_container = QHBoxLayout(mode_widget)
        layout.addWidget(mode_widget)

        modes = {
            "performance": ("Performance", "performance.png"),
            "balanced": ("Balanced", "balanced.png"),
            "default": ("Default", "default.png")
        }

        for mode_key, (label_text, image_file) in modes.items():
            outer_layout = QVBoxLayout()
            outer_layout.setAlignment(Qt.AlignmentFlag.AlignCenter)
            outer_layout.setSpacing(8)

            label = QLabel(label_text)
            label.setFont(QFont("ElysiaOSNew", 12))
            label.setStyleSheet("color: white; background: transparent; border: none; margin: 0px; padding: 0px")
            label.setAlignment(Qt.AlignmentFlag.AlignCenter)

            frame = QFrame(self.power_box)
            frame.setFixedSize(130, 130)
            frame.setStyleSheet("color: white; background: transparent; border: none; margin: 0px; padding: 0px")
            frame.setCursor(Qt.CursorShape.PointingHandCursor)

            stacked_layout = QStackedLayout(frame)

            image_label = QLabel()
            image_path = os.path.expanduser(f"~/Elysia/assets/assets/{image_file}")
            if os.path.exists(image_path):
                pixmap = QPixmap(image_path).scaled(130, 130, Qt.AspectRatioMode.KeepAspectRatio, Qt.TransformationMode.SmoothTransformation)
                image_label.setPixmap(pixmap)
            else:
                image_label.setText("Missing")

            tick = QLabel("✔")
            tick.setStyleSheet("color: #e5a7c6; font-size: 24px; background: transparent; border: none;")
            tick.setAlignment(Qt.AlignmentFlag.AlignTop | Qt.AlignmentFlag.AlignRight)
            tick.setVisible(False)

            stacked_layout.addWidget(image_label)
            stacked_layout.addWidget(tick)

            frame.mousePressEvent = lambda _, m=mode_key: self.select_mode(m)
            self.mode_boxes[mode_key] = (frame, tick)

            outer_layout.addWidget(label)
            outer_layout.addWidget(frame)

            mode_container.addLayout(outer_layout)

        layout.addLayout(mode_container)

        self.apply_btn = QPushButton("Apply Changes", self.power_box)
        self.apply_btn.setFixedSize(200, 50)
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
        self.apply_btn.clicked.connect(self.apply_changes)
        self.apply_btn.setGeometry(250, 370, 200, 150)

    def show(self):
        self.parent.hide_tiles()
        self.parent.show_back_button()
        self.power_box.setVisible(True)
        self.auto_detect_current_mode()

    def hide(self):
        self.power_box.setVisible(False)
        self.parent.hide_back_button()
        self.parent.show_tiles()

    def select_mode(self, mode):
        self.selected_mode = mode
        for name, (frame, tick) in self.mode_boxes.items():
            is_selected = name == mode
            frame.setStyleSheet(f"border: {'3px solid #e5a7c6' if is_selected else '2px solid transparent'}; border-radius: 26px; background: transparent;")
            tick.setVisible(is_selected)

    def apply_changes(self):
        if not self.selected_mode:
            return

        command = {
            "performance": "pkexec auto-cpufreq --force=performance",
            "balanced": "pkexec auto-cpufreq --force=powersave",
            "default": "pkexec auto-cpufreq --force=reset"
        }.get(self.selected_mode)

        if command:
            subprocess.Popen(command.split())

    def auto_detect_current_mode(self):
        try:
            output = subprocess.check_output(["pkexec", "auto-cpufreq", "--debug"], stderr=subprocess.STDOUT, text=True, timeout=15)
            if "Currently using: performance governor" in output:
                self.select_mode("performance")
            elif "Currently using: powersave governor" in output:
                self.select_mode("balanced")
            elif "Currently using: reset governor" in output:
                self.select_mode("default")
        except subprocess.SubprocessError:
            pass
