import subprocess, re
from PyQt6.QtWidgets import QLabel, QWidget, QComboBox, QSlider, QPushButton, QMessageBox
from PyQt6.QtGui import QFont
from PyQt6.QtCore import Qt, QTimer


class SoundManager:
    def __init__(self, parent):
        self.parent = parent
        self.audio_timer, self.mic_timer = None, None
        self.refresh_timer = QTimer()
        self.refresh_timer.timeout.connect(self.refresh_state)
        self.playback_sliders = []
        self.playback_cache = {}
        self.init_ui()

    def run_cmd(self, cmd):
        try:
            result = subprocess.run(cmd, shell=True, capture_output=True, text=True, timeout=10)
            return result.stdout.strip() if result.returncode == 0 else ""
        except Exception as e:
            print(f"Command error: {e}")
            return ""

    def dropdown_style(self):
        return """
            QComboBox {
                background: rgba(255, 255, 255, 0.2);
                color: white;
                border: 1px solid rgba(255, 255, 255, 0.2);
                border-radius: 36px;
                padding: 5px;
                font-size: 16px;
            }
            QComboBox QAbstractItemView {
                background: rgba(0, 0, 0, 0.2);
                border: 1px solid rgba(255, 255, 255, 0.2);
                color: black;
                padding: 5px;
                margin: 0px;
                selection-background-color: transparent;
            }
        """

    def slider_style(self):
        return """
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
                border-radius: 4px;
            }
        """

    def init_ui(self):
        self.sound_box = QWidget(self.parent)
        self.sound_box.setGeometry(500, 100, 600, 600)
        self.sound_box.setStyleSheet("background-color: rgba(255,255,255,0.05); border: 2px solid rgba(255,255,255,0.15); border-radius: 30px;")
        self.sound_box.setVisible(False)

        font = QFont("ElysiaOSNew", 14)

        def add_label(text, y):
            label = QLabel(text, self.sound_box)
            label.setFont(font)
            label.setStyleSheet("color: white; background: transparent; border: none; margin: 0px; padding: 0px")
            label.move(220, y)
            return label

        def add_dropdown(y, callback):
            dropdown = QComboBox(self.sound_box)
            dropdown.setGeometry(150, y, 300, 40)
            dropdown.setStyleSheet(self.dropdown_style())
            dropdown.currentIndexChanged.connect(callback)
            return dropdown

        def add_slider(y, change_cb):
            slider = QSlider(Qt.Orientation.Horizontal, self.sound_box)
            slider.setGeometry(100, y, 400, 30)
            slider.setMinimum(0)
            slider.setMaximum(100)
            slider.setStyleSheet(self.slider_style())
            slider.valueChanged.connect(change_cb)
            return slider

        self.audio_label = add_label("Audio Output", 30)
        self.audio_dropdown = add_dropdown(60, self.set_audio_output)
        self.audio_slider = add_slider(110, self.debounce_audio)

        self.mic_label = add_label("Microphone Input", 170)
        self.mic_dropdown = add_dropdown(200, self.set_mic_input)
        self.mic_slider = add_slider(250, self.debounce_mic)

        self.apply_btn = QPushButton("Apply Changes", self.sound_box)
        self.apply_btn.setGeometry(200, 510, 200, 50)
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

    def get_ports_for_device(self, name, type_):
        lines = self.run_cmd(f"pactl list {type_}s").splitlines()
        ports, in_block, in_ports = [], False, False
        for line in lines:
            if f"Name: {name}" in line:
                in_block = True
            elif in_block and f"{type_.capitalize()} #" in line:
                break
            elif in_block and "Ports:" in line:
                in_ports = True
            elif in_ports and line.strip() == "":
                in_ports = False
            elif in_ports and ':' in line:
                match = re.match(r'\s*(\S+):\s*([^(]+)', line)
                if match:
                    ports.append((match.group(1), match.group(2).strip()))
        return ports

    def simplify_description(self, text):
        text = text.replace("Analog Stereo", "")
        text = re.sub(r'Family\s+[\w/]+?\s+HD\s+Audio\s+Controller', '', text, flags=re.IGNORECASE)
        text = re.sub(r'[\s\-]+$', '', text)
        return text.strip()

    def get_devices(self, type_):
        short = self.run_cmd(f"pactl list short {type_}s")
        full = self.run_cmd(f"pactl list {type_}s")
        devices = []
        for line in short.splitlines():
            parts = line.split("\t")
            if len(parts) < 2: continue
            dev_id, dev_name = parts[0], parts[1]
            desc = dev_name
            for block in full.split(f"{type_.capitalize()} #")[1:]:
                if f"Name: {dev_name}" in block:
                    for l in block.splitlines():
                        if "Description:" in l:
                            raw = l.split("Description:")[1].strip()
                            desc = self.simplify_description(raw)
                            break
            ports = self.get_ports_for_device(dev_name, type_)
            active = self.get_active_port(dev_name, type_)
            devices.append({"id": dev_id, "name": dev_name, "description": desc, "ports": ports, "active": active})
        return devices

    def get_active_port(self, dev_name, type_):
        lines = self.run_cmd(f"pactl list {type_}s").splitlines()
        found = False
        for line in lines:
            if f"Name: {dev_name}" in line:
                found = True
            elif found and "Active Port:" in line:
                return line.split(":")[1].strip()
            elif found and f"{type_.capitalize()} #" in line:
                break
        return None

    def set_audio_output(self):
        item = self.audio_dropdown.currentData()
        if item:
            device, port = item
            self.run_cmd(f"pactl set-default-sink '{device}'")
            self.run_cmd(f"pactl set-sink-port '{device}' '{port}'")

    def set_mic_input(self):
        item = self.mic_dropdown.currentData()
        if item:
            device, port = item
            self.run_cmd(f"pactl set-default-source '{device}'")
            self.run_cmd(f"pactl set-source-port '{device}' '{port}'")

    def set_audio_volume(self, val):
        item = self.audio_dropdown.currentData()
        if item:
            device, _ = item
            self.run_cmd(f"pactl set-sink-volume '{device}' {val}%")

    def set_mic_volume(self, val):
        item = self.mic_dropdown.currentData()
        if item:
            device, _ = item
            self.run_cmd(f"pactl set-source-volume '{device}' {val}%")

    def debounce_audio(self, val):
        if self.audio_timer: self.audio_timer.stop()
        self.audio_timer = QTimer(singleShot=True, timeout=lambda: self.set_audio_volume(val))
        self.audio_timer.start(100)

    def debounce_mic(self, val):
        if self.mic_timer: self.mic_timer.stop()
        self.mic_timer = QTimer(singleShot=True, timeout=lambda: self.set_mic_volume(val))
        self.mic_timer.start(100)

    def apply_changes(self):
        try:
            self.set_audio_output()
            self.set_mic_input()
            self.set_audio_volume(self.audio_slider.value())
            self.set_mic_volume(self.mic_slider.value())
            QMessageBox.information(self.parent, "Audio", "Settings applied.")
        except Exception as e:
            QMessageBox.warning(self.parent, "Error", f"Failed to apply: {str(e)}")

    def refresh_state(self):
        self.update_slider(self.audio_dropdown, self.audio_slider, is_sink=True)
        self.update_slider(self.mic_dropdown, self.mic_slider, is_sink=False)
        self.refresh_playback_sliders()

    def update_slider(self, dropdown, slider, is_sink):
        item = dropdown.currentData()
        if not item: return
        device, _ = item
        kind = "sink" if is_sink else "source"
        out = self.run_cmd(f"pactl get-{kind}-volume '{device}'")
        match = re.search(r'(\d+)%', out)
        if match:
            vol = min(int(match.group(1)), 100)
            if slider.value() != vol:
                slider.blockSignals(True)
                slider.setValue(vol)
                slider.blockSignals(False)

    def get_playback_clients(self):
        output = self.run_cmd("pactl list sink-inputs")
        clients, current = [], {}
        for line in output.splitlines():
            line = line.strip()
            if line.startswith("Sink Input #"):
                if current: clients.append(current)
                current = {"index": line.split("#")[1]}
            elif line.startswith("application.name"):
                match = re.search(r'"([^"]+)"', line)
                if match: current["name"] = match.group(1)
            elif line.startswith("Volume:"):
                match = re.findall(r'(\d+)%', line)
                if match:
                    volumes = list(map(int, match))
                    avg_volume = sum(volumes) // len(volumes)
                    current["volume"] = avg_volume
        if current: clients.append(current)
        return clients

    def refresh_playback_sliders(self):
        for slider, label in self.playback_sliders:
            slider.deleteLater()
            label.deleteLater()
        self.playback_sliders.clear()

        y = 320
        font = QFont("ElysiaOSNew", 13)
        for client in self.get_playback_clients():
            label = QLabel(client.get("name", f"App {client['index']}"), self.sound_box)
            label.setFont(font)
            label.setStyleSheet("color: white; background: transparent; border: none; margin: 0px; padding: 0px")
            label.move(10, y)

            slider = QSlider(Qt.Orientation.Horizontal, self.sound_box)
            slider.setGeometry(150, y, 400, 25)
            slider.setMinimum(0)
            slider.setMaximum(100)
            slider.setValue(client["volume"])
            slider.setStyleSheet(self.slider_style())
            slider.valueChanged.connect(lambda val, idx=client["index"]: self.run_cmd(f"pactl set-sink-input-volume {idx} {val}%"))
            slider.show()

            label.show()
            self.playback_sliders.append((slider, label))
            y += 40

    def show(self):
        self.audio_dropdown.clear()
        self.mic_dropdown.clear()

        for dev in self.get_devices("sink"):
            for port, desc in dev["ports"]:
                label = f"{dev['description']} - {desc}"
                self.audio_dropdown.addItem(label, (dev["name"], port))
                if dev["active"] == port:
                    self.audio_dropdown.setCurrentIndex(self.audio_dropdown.count() - 1)

        for dev in self.get_devices("source"):
            if ".monitor" in dev["name"]: continue
            for port, desc in dev["ports"]:
                label = f"{dev['description']} - {desc}"
                self.mic_dropdown.addItem(label, (dev["name"], port))
                if dev["active"] == port:
                    self.mic_dropdown.setCurrentIndex(self.mic_dropdown.count() - 1)

        self.refresh_state()
        self.sound_box.setVisible(True)
        self.parent.hide_tiles()
        self.parent.show_back_button()
        self.refresh_timer.start(1000)

    def hide(self):
        if self.audio_timer: self.audio_timer.stop()
        if self.mic_timer: self.mic_timer.stop()
        self.refresh_timer.stop()
        self.sound_box.setVisible(False)
        self.parent.hide_back_button()
        self.parent.show_tiles()
