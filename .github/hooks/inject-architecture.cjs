#!/usr/bin/env node
// SessionStart: добавляет краткую карту воркспейса и индекс ADR
// в контекст первой сессии. Дешевле, чем грузить в copilot-instructions.md.

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

  const cwd = event.cwd || process.cwd();
  const context = [];

  context.push('# Карта воркспейса arm (краткая)');
  context.push('');
  context.push('Три пакета по ответственности:');
  context.push('- `arm_description` — xacro, world Gazebo, RViz, launch визуализации/симуляции (ament_python).');
  context.push('- `arm_moveit_config` — SRDF, kinematics, joint_limits, controllers, launch MoveIt 2 (ament_cmake).');
  context.push('- `arm_control` — команды движения через MoveIt 2, слои domain/ports/application/adapters/apps (ament_cmake).');
  context.push('');
  context.push('Имя робота, группы и контроллера — `arm`. Группа кисти — `wrist`. Фланцы — `base_link` (корень), `tcp` (инструмент).');
  context.push('Старые имена `two_link_*` не поддерживаются.');
  context.push('');

  // Индекс ADR, если файл существует
  const adrIndex = path.join(cwd, 'docs', 'decisions', 'README.md');
  if (fs.existsSync(adrIndex)) {
    context.push('## ADR — обязательное чтение перед изменениями');
    context.push('');
    context.push('| ADR | Решение |');
    context.push('|---|---|');
    context.push('| ADR-001 | Разделение воркспейса на три пакета |');
    context.push('| ADR-002 | Слои domain/ports/application/adapters в arm_control |');
    context.push('| ADR-003 | config/motion.yaml — единственный источник настроек движения |');
    context.push('| ADR-004 | Один блок ros2_control, режим через use_gazebo |');
    context.push('| ADR-005 | Очистка XML-комментариев, запрет онлайн-базы моделей Gazebo |');
    context.push('| ADR-006 | move_group поверх Gazebo работает на часах симуляции |');
    context.push('');
    context.push('Подробности — `decisions/ADR-*.md`. Прочитай релевантный ADR через `#file:` перед изменением.');
  }

  process.stdout.write(JSON.stringify({
    hookSpecificOutput: {
      hookEventName: 'SessionStart',
      additionalContext: context.join('\n')
    }
  }));
  process.exit(0);
});