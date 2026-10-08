#!/usr/bin/env node
// PostToolUse: запускает ament_flake8/ament_pep257 при правке Python-файлов.
// Результат добавляется в контекст модели через additionalContext.

const fs = require('node:fs');
const path = require('node:path');
const { spawnSync } = require('node:child_process');

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
  if (!filePath.endsWith('.py')) process.exit(0);

  // Не проверяем сам hooks
  if (filePath.includes('.github/hooks/')) process.exit(0);

  const abs = path.isAbsolute(filePath) ? filePath : path.resolve(event.cwd || '.', filePath);
  if (!fs.existsSync(abs)) process.exit(0);

  // Пробуем ament_flake8, если доступен
  const flake = spawnSync('ament_flake8', [abs], { encoding: 'utf8', timeout: 20000 });
  const pep = spawnSync('ament_pep257', [abs], { encoding: 'utf8', timeout: 20000 });

  const messages = [];
  if (flake.status !== 0 && flake.stdout) {
    messages.push('=== ament_flake8 ===\n' + flake.stdout);
  }
  if (pep.status !== 0 && pep.stdout) {
    messages.push('=== ament_pep257 ===\n' + pep.stdout);
  }

  if (messages.length === 0) process.exit(0);

  process.stdout.write(JSON.stringify({
    hookSpecificOutput: {
      hookEventName: 'PostToolUse',
      additionalContext:
        'Линтеры ROS 2 обнаружили проблемы в изменённом Python-файле (см. code_quality.instructions.md):\n\n' +
        messages.join('\n\n') +
        '\n\nИсправь замечания перед завершением задачи.'
    }
  }));
  process.exit(0);
});