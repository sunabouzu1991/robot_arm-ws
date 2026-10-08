#!/usr/bin/env node
// ADR-006: запрещает редактирование сгенерированных Setup Assistant файлов.

const GENERATED_FILES = [
  'move_group.launch.py',
  'moveit_rviz.launch.py',
  'setup_assistant.launch.py',
  'initial_positions.yaml',
  'moveit.rviz',
];

const ALLOWED_DIRS = [
  'launch/',
  'config/',
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

  // Срабатываем только на редактирование
  const toolName = event?.tool_name || '';
  if (!/edit|create/i.test(toolName)) process.exit(0);

  const filePath = event?.tool_input?.file_path || event?.tool_input?.path || '';
  if (!filePath) process.exit(0);

  const base = filePath.split('/').pop();
  if (!GENERATED_FILES.includes(base)) process.exit(0);

  // Разрешаем, если это не в arm_moveit_config (маловероятно, но на всякий случай)
  if (!filePath.includes('arm_moveit_config')) process.exit(0);

  process.stdout.write(JSON.stringify({
    hookSpecificOutput: {
      hookEventName: 'PreToolUse',
      permissionDecision: 'deny',
      permissionDecisionReason:
        `ADR-006: файл ${base} сгенерирован MoveIt Setup Assistant и будет перезаписан при следующем запуске мастера. ` +
        'Правки будут потеряны. Вместо этого:\n' +
        '  - для mock-режима правь demo.launch.py (ручной);\n' +
        '  - для Gazebo используй gazebo_demo.launch.py (ручной, с use_sim_time);\n' +
        '  - для изменения конфигурации — config/*.yaml.'
    }
  }));
  process.exit(0);
});