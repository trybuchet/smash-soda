const fs = require('fs');
const path = require('path');

const ROOT = __dirname;
const VERSION_PATTERN = /^\d+\.\d+\.\d+(?:[-+][0-9A-Za-z.-]+)?$/;
const VERSION_DECLARATION = /(inline constexpr char Version\[\] = )"[^"]*";/g;
const CONFIG_FILES = [
  path.join(ROOT, 'SmashSoda', 'PrivateConfig.example.h'),
  path.join(ROOT, 'SmashSoda', 'PrivateConfig.h'),
];

function parseVersion(argv) {
  const args = argv.filter(Boolean);
  const versionArg = args.find((arg) => !arg.startsWith('--'))
    || args.find((arg) => arg.startsWith('--version='))?.slice('--version='.length);

  if (!versionArg) {
    throw new Error('Pass a version, for example: 7.0.5');
  }
  if (!VERSION_PATTERN.test(versionArg)) {
    throw new Error(`Invalid version "${versionArg}". Expected a version like 7.0.5 or 7.0.5-beta.1.`);
  }

  return versionArg;
}

function updateVersion(version) {
  const files = CONFIG_FILES.filter((file) => fs.existsSync(file));
  const templatePath = CONFIG_FILES[0];
  if (!fs.existsSync(templatePath)) {
    throw new Error('Missing Smash Soda version template: SmashSoda/PrivateConfig.example.h');
  }

  const updates = files.map((file) => {
    const content = fs.readFileSync(file, 'utf8');
    const matches = [...content.matchAll(VERSION_DECLARATION)];
    if (matches.length !== 1) {
      throw new Error(`Expected exactly one Version setting in ${path.relative(ROOT, file)}; found ${matches.length}.`);
    }

    return {
      file,
      content: content.replace(VERSION_DECLARATION, `$1"${version}";`),
    };
  });

  for (const { file, content } of updates) {
    if (content !== fs.readFileSync(file, 'utf8')) {
      fs.writeFileSync(file, content, 'utf8');
      console.log(`Updated ${path.relative(ROOT, file)}`);
    }
  }
}

try {
  const version = parseVersion(process.argv.slice(2));
  updateVersion(version);
} catch (error) {
  console.error(`Smash Soda version update failed: ${error.message}`);
  process.exit(1);
}
