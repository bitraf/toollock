# State machine logic for the tool lock
finito = require 'finitosm'

lock = finito.define 'Tool lock'
  .state 'start'
  .state 'locked'
  .state 'takeTool'
  .state 'toolTaken'

  .transition()
    .from('start').to('locked')._when('key_present')
  .transition()
    .from('start').to('toolTaken')._when('no_key')

  # claiming the tool
  .transition()
    .from('locked').to('takeTool')._when('mqtt_unlock()')
  .transition()
    .from('takeTool').to('locked')._when('timeout_key_present')
  .transition()
    .from('takeTool').to('toolTaken')._when('no_key')

  # returning the tool
  .transition()
    .from('toolTaken').to('locked')._when('key_present')

  .initial 'start'
  .done()

module.exports = lock
if not module.parent
  p = JSON.stringify(lock.data, null, 2)
  console.log(p)
