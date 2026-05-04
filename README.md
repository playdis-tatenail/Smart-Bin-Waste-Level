# Smart-Bin-Waste-Level
ระบบตรวจวัดระดับขยะในถังแบบเรียลไทม์โดยใช้เทคโนโลยี IoT เพื่อแก้ไขปัญหาขยะล้นถังและช่วยให้เจ้าหน้าที่สามารถวางแผนการเก็บขยะได้
## Features 🌟
- Real-time Monitoring: วัดความสูงของขยะภายในถังด้วยเซนเซอร์ Ultrasonic
- Fill Rate Analysis: วิเคราะห์ความเร็วในการเพิ่มขึ้นของปริมาณขยะต่อนาที
- Waste Prediction: พยากรณ์ระยะเวลาที่ขยะจะเต็มถังล่วงหน้า 
- Automated Alerts: แจ้งเตือนผ่าน Discord Webhook เมื่อขยะเกิน 80% และแจ้งเมื่อมีการเคลียร์ขยะแล้ว (<20%)
- Data Visualization: ส่งข้อมูลเข้า Google Sheets อัตโนมัติและแสดงผลผ่านกราฟบน Looker Studio

## Hardware 🛠️
- ESP32: ไมโครคอนโทรลเลอร์หลักรองรับ Wi-Fi
- Ultrasonic Sensor (HC-SR04): ใช้ส่งคลื่นเสียงเพื่อวัดระยะห่างระหว่างฝาถังกับผิวขยะ
- สายไฟจัมเปอร์ และแหล่งจ่ายไฟ

## Software & Tools 💻
- Arduino IDE: สำหรับเขียนโปรแกรมควบคุมบอร์ด ESP32
- Discord Webhook API: สำหรับระบบแจ้งเตือนเข้าสมาร์ทโฟนหรือคอมพิวเตอร์
- Google Sheets & Apps Script: สำหรับเก็บข้อมูล Log การทำงาน
- Looker Studio: สำหรับสรุปผลข้อมูลในรูปแบบ Dashboard
- Wokwi Simulator: สำหรับจำลองการทำงานของวงจร

## Pin Assignment 🔌
Ultrasonic Sensor (HC-SR04)    ESP32 GPIO
Trig (Trigger)           --->  GPIO 5   
Echo                     --->  GPIO 18 
