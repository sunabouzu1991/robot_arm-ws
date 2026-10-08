#!/usr/bin/env node
// Проверяет ADR-005: запрещает ": " внутри XML-комментариев в xacro/urdf/world.
// Срабатывает после PostToolUse (edit/createFile).

const fs = require('node:fs');
const path = require('node:path');

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

  const relevantExt = ['.xacro', '.urdf', '.world', '.sdf'];
  if (!relevantExt.some(ext => filePath.endsWith(ext))) {
    process.exit(0);
  }

  const abs = path.isAbsolute(filePath) ? filePath : path.resolve(event.cwd || '.', filePath);
  if (!fs.existsSync(abs)) process.exit(0);

  const content = fs.readFileSync(abs, 'utf8');
  const offenders = [];

  // Ищем <!-- ... --> и проверяем, есть ли внутри ": "
  const commentRegex = /<!--([\s\S]*?)-->/g;
  let match;
  while ((match = commentRegex.exec(content)) !== null) {
    if (/: /.test(match[1])) {
      const lineNo = content.slice(0, match.index).split('\n').length;
      offenders.push({ line: lineNo, snippet: match[0].slice(0, 80) });
    }
  }

  if (offenders.length === 0) process.exit(0);

  const lines = offenders.map(o =>
    `  строка ${o.line}: ${o.snippet}`
  ).join('\n');

  const output = {
    decision: 'block',
    reason:
      'ADR-005: обнаружены ": " внутри XML-комментариев. ' +
      'Это ломает --param robot_description:=<urdf> в gazebo_ros2_control (Humble): ' +
      'rcl парсит значение как YAML plain-scalar и падает с "mapping values are not allowed here".\n' +
      `Найдено:\n${lines}\n` +
      'Исправь: удали ": " из комментария или переформулируй без двоеточия.'
  };

  process.stdout.write(JSON.stringify(output));
  process.exit(2); // blocking error — stderr пойдёт модели
});