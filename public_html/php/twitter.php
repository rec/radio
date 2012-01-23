<div id="twitter" class="box">
<script charset="utf-8" src="http://widgets.twimg.com/j/2/widget.js"></script>
<script>
  new TWTR.Widget({
    version: 2,
    type: 'profile',
    rpp: 4,
    interval: 30000,
    width: "auto",
    theme: {
      shell: {
        background: '#333333',
        color: '#ffffff'
      },
      tweets: {
        background: '#5c5c5c',
        color: '#ffffff',
        links: '#ebbd07'
      }
    },
    features: {
      scrollbar: false,
      loop: false,
      live: false,
      behavior: 'all'
    }
  }).render().setUser('swirlyRadio').start();
  </script>

</div>
