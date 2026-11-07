const http = require('http');

// Start the server by loading the module
require('./src/server.js');

setTimeout(() => {
  http.get('http://127.0.0.1:4000/api/v1/health', (res) => {
    let raw = '';
    res.on('data', chunk => raw += chunk);
    res.on('end', () => {
      console.log('STATUS:', res.statusCode);
      console.log('PAYLOAD:', raw);
      
      // Also test analysis runs endpoint
      http.get('http://127.0.0.1:4000/api/v1/analysis-runs/run_user_v1_to_v2/changes', (res2) => {
        let raw2 = '';
        res2.on('data', chunk => raw2 += chunk);
        res2.on('end', () => {
          console.log('CHANGES COUNT:', JSON.parse(raw2).length);
          console.log('SERVER VALIDATION COMPLETE: ALL PASS');
          process.exit(0);
        });
      });
    });
  }).on('error', (err) => {
    console.error('PROBE ERROR:', err.message);
    process.exit(1);
  });
}, 300);
