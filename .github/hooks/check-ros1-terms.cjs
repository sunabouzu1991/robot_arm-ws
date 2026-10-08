#!/usr/bin/env node
// Блокирует использование ROS 1 терминов в коде воркспейса.

const fs = require('node:fs');
const path = require('node:path');

const ROS1_PATTERNS = [
  { re: /\bcatkin_make\b/, name: 'catkin_make' },
  { re: /\bcatkin\b/, name: 'catkin' },
  { re: /\brosbuild\b/, name: 'rosbuild' },
  { re: /\brospy\b/, name: 'rospy' },
  { re: /\broscpp\b/, name: 'roscpp' },
  { re: /#include\s*<ros\//, name: '#include <ros/...>' },
  { re: /\bros::init\b/, name: 'ros::init' },
  { re: /\bNodeHandle\b/, name: 'NodeHandle' },
];

let input = '';
process.stdin.setEncoding('utf8');
process.stdin.on('data', chunk => input += chunk);
process.stdin.on('end', () => {
  let event;
  try {
    event = JSON.parse(input);
  } catch {
    process.exit(0);
  }

  const filePath = event?.tool_input?.file_path || event?.tool_input?.path || '';
  if (!filePath) process.exit(0);

  const relevantExt = ['.cpp', '.hpp', '.h', '.cc', '.py', '.xml', '.xacro', '.urdf', '.txt'];
  if (!relevantExt.some(ext => filePath.endsWith(ext))) {
    process.exit(0);
  }

  // Исключаем сами hooks и docs
  if (filePath.includes('.github/hooks/') || filePath.endsWith('.md')) {
    process.exit(0);
  }

  const abs = path.isAbsolute(filePath) ? filePath : path.resolve(event.cwd || '.', filePath);
  if (!fs.existsSync(abs)) process.exit(0);

  const content = fs.readFileSync(abs, 'utf8');
  const offenders = [];

  for (const { re, name } of ROS1_PATTERNS) {
    const m = re.exec(content);
    if (m) {
      const lineNo = content.slice(0, m.index).split('\n').length;
      offenders.push({ line: lineNo, name });
    }
  }

  if (offenders.length === 0) process.exit(0);

  const lines = offenders.map(o => `  строка ${o.line}: ${o.name}`).join('\n');

  process.stdout.write(JSON.stringify({
    decision: 'block',
    reason:
      'Обнаружены ROS 1 артефакты. Проект строго на ROS 2 (см. copilot-instructions.md).\n' +
      `Найдено:\n${lines}\n` +
      'Замени: catkin_make → colcon build, rospy → rclpy, roscpp → rclcpp, #include <ros/...> → #include <rclcpp/...>.'
  }));
  process.exit(2);
});