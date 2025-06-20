import subprocess
import os
from PyQt6.QtWidgets import QLabel, QWidget
from PyQt6.QtGui import QFont, QPixmap
from PyQt6.QtCore import Qt

ASSETS_PATH = os.path.expanduser("~/Elysia/assets/assets")
FASTFETCH_CMD = ["fastfetch", "-c", os.path.expanduser("~/.config/fastfetch/sys-config.jsonc")]


class AboutManager:
    def __init__(self, parent):
        self.parent = parent
        self.labels = []

        self.about_box = QWidget(self.parent)
        self.about_box.setGeometry(500, 100, 600, 640)
        self.about_box.setStyleSheet("""
            background-color: rgba(255, 255, 255, 0.05);
            border: 2px solid rgba(255, 255, 255, 0.15);
            border-radius: 30px;
        """)
        self.about_box.setVisible(False)

        # Desktop image
        self.image = QLabel(self.about_box)
        self.image.setPixmap(QPixmap(os.path.join(ASSETS_PATH, "desktop.png")).scaled(120, 120, Qt.AspectRatioMode.KeepAspectRatio, Qt.TransformationMode.SmoothTransformation))
        self.image.setGeometry(0, 0, 580, 170)
        self.image.setStyleSheet("color: white; background: transparent; border: none; margin: 0px; padding: 0px")
        self.image.setAlignment(Qt.AlignmentFlag.AlignCenter)

        # Title
        title = QLabel("SYSTEM INFORMATION", self.about_box)
        title.setFont(QFont("ElysiaOSNew", 20, QFont.Weight.Bold))
        title.setStyleSheet("color: white; background: transparent; border: none; margin: 0px; padding: 0px")
        title.setAlignment(Qt.AlignmentFlag.AlignCenter)
        title.setGeometry(155, 90, 290, 170)

        # Fastfetch output labels
        self.fastfetch_lines = self.get_fastfetch_output()
        y = 210
        for line in self.fastfetch_lines:
            label = QLabel(line.strip(), self.about_box)
            label.setFont(QFont("ElysiaOSNew22", 16))
            label.setStyleSheet("color: white; background: transparent; border: none; margin: 0px; padding: 0px")
            label.setGeometry(70, y, 480, 30)
            label.setWordWrap(True)
            label.setAlignment(Qt.AlignmentFlag.AlignLeft)
            label.setVisible(True)
            self.labels.append(label)
            y += 30

    def get_fastfetch_output(self):
        try:
            output = subprocess.check_output(FASTFETCH_CMD).decode()
            return [line for line in output.strip().splitlines() if line.strip()]
        except Exception as e:
            return [f"Error: {e}"]

    def show(self):
        self.parent.hide_tiles()
        self.about_box.setVisible(True)
        self.parent.show_back_button()

    def hide(self):
        self.about_box.setVisible(False)
        self.parent.hide_back_button()
        self.parent.show_tiles()
