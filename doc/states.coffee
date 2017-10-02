# State machine logic for the tool lock
finito = require 'finitosm'

lock = finito.define 'Tool lock'
  .state 'start'
  .state 'toolPresent'
  .state 'toolTaken'
  .state 'takeTool'

  .transition()
    .from('start').to('toolPresent')._when('key_present')
  .transition()
    .from('start').to('toolTaken')._when('no_key')

  # claiming the tool
  .transition()
    .from('toolPresent').to('takeTool')._when('mqtt_unlock()')
  .transition()
    .from('takeTool').to('toolPresent')._when('timeout_key_present')
  .transition()
    .from('takeTool').to('toolTaken')._when('timeout_no_key')

  # returning the tool
  .transition()
    .from('toolTaken').to('toolPresent')._when('key_present')

  .initial 'start'
  .done()

module.exports = lock
if not module.parent
  p = JSON.stringify(lock.data, null, 2)
  console.log(p)
