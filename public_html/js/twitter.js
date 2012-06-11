function startTwitter() {
  new TWTR.Widget({
    version: 2,
    type: 'profile',
    rpp: 5,
    interval: Common.Delay.twitter,
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
};

startTwitter();